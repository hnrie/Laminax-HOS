/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include <string.h>

#include <glib/gi18n-lib.h>
#include <glib/gstdio.h>

#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-desktopappinfo.h>

#include <libxapp/xapp-gpu-offload-helper.h>

#include <meta/display.h>
#include <meta/meta-workspace-manager.h>

#include "Laminax-app-private.h"
#include "Laminax-enum-types.h"
#include "Laminax-global-private.h"
#include "Laminax-util.h"
#include "Laminax-app-system-private.h"
#include "Laminax-window-tracker-private.h"
#include "st.h"

/* This is mainly a memory usage optimization - the user is going to
 * be running far fewer of the applications at one time than they have
 * installed.  But it also just helps keep the code more logically
 * separated.
 */
typedef struct {
  guint refcount;

  /* Signal connection to dirty window sort list on workspace changes */
  guint workspace_switch_id;

  GSList *windows;

  /* Whether or not we need to resort the windows; this is done on demand */
  guint window_sort_stale : 1;
} LaminaxAppRunningState;

/**
 * SECTION:Laminax-app
 * @short_description: Object representing an application
 *
 * This object wraps a #GMenuTreeEntry, providing methods and signals
 * primarily useful for running applications.
 */
struct _LaminaxApp
{
  GObject parent;

  LaminaxGlobal *global;

  int started_on_workspace;

  LaminaxAppState state;

  GMenuTreeEntry *entry; /* If NULL, this app is backed by one or more
                          * MetaWindow.  For purposes of app title
                          * etc., we use the first window added,
                          * because it's most likely to be what we
                          * want (e.g. it will be of TYPE_NORMAL from
                          * the way Laminax-window-tracker.c works).
                          */
  GMenuDesktopAppInfo *info;

  LaminaxAppRunningState *running_state;

  char *window_id_string;

  char *keywords;
  char *unique_name;

  gboolean hidden_as_duplicate;
  gboolean is_flatpak;
};

G_DEFINE_TYPE (LaminaxApp, Laminax_app, G_TYPE_OBJECT);

enum {
  PROP_0,
  PROP_STATE
};

enum {
  WINDOWS_CHANGED,
  LAST_SIGNAL
};

static guint Laminax_app_signals[LAST_SIGNAL] = { 0 };

static void create_running_state (LaminaxApp *app);
static void unref_running_state (LaminaxAppRunningState *state);

static void
Laminax_app_get_property (GObject    *gobject,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  LaminaxApp *app = Laminax_APP (gobject);

  switch (prop_id)
    {
    case PROP_STATE:
      g_value_set_enum (value, app->state);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

const char *
Laminax_app_get_id (LaminaxApp *app)
{
  if (app->entry)
    return gmenu_tree_entry_get_desktop_file_id (app->entry);
  return app->window_id_string;
}

char *
Laminax_app_get_flatpak_app_id (LaminaxApp *app)
{
  if (app->info)
  {
    gchar *id;

    id = g_strdup (gmenu_desktopappinfo_get_flatpak_app_id (app->info));

    if (id != NULL)
    {
        return id;
    }
    else
    {
        const gchar *desktop_file = Laminax_app_get_id (app);

        gchar **split = g_strsplit (desktop_file, ".desktop", -1);
        id = g_strdup (split[0]);
        g_strfreev (split);

        return id;
    }
  }

  // This should never occur
  return NULL;
}

static MetaWindow *
window_backed_app_get_window (LaminaxApp     *app)
{
  g_assert (app->info == NULL);
  if (app->running_state)
    {
      g_assert (app->running_state->windows);
      return app->running_state->windows->data;
    }
  else
    return NULL;
}

static ClutterActor *
get_actor_for_icon_name (LaminaxApp *app,
                         const gchar *icon_name,
                         gint         size)
{
  ClutterActor *actor;
  GIcon *icon;

  icon = NULL;
  actor = NULL;

  if (g_path_is_absolute (icon_name))
    {
      GFile *icon_file;

      icon_file = g_file_new_for_path (icon_name);
      icon = g_file_icon_new (icon_file);

      g_object_unref (icon_file);
    }
  else
    {
      icon = g_themed_icon_new (icon_name);
    }

  if (icon != NULL)
  {
    actor = g_object_new (ST_TYPE_ICON, "gicon", icon, "icon-type", ST_ICON_FULLCOLOR, "icon-size", size, NULL);
    g_object_unref (icon);
  }

  return actor;
}

static ClutterActor *
get_failsafe_icon (int size)
{
  GIcon *icon = g_themed_icon_new ("application-x-executable");
  ClutterActor *actor = g_object_new (ST_TYPE_ICON, "gicon", icon, "icon-type", ST_ICON_FULLCOLOR, "icon-size", size, NULL);
  g_object_unref (icon);
  return actor;
}


static ClutterActor *
window_backed_app_get_icon (LaminaxApp *app,
                            int       size)
{
  MetaWindow *window = NULL;
  StWidget *widget;
  int scale, scaled_size;
  LaminaxGlobal *global;
  StThemeContext *context;

  global = Laminax_global_get ();
  context = st_theme_context_get_for_stage (Laminax_global_get_stage (global));
  g_object_get (context, "scale-factor", &scale, NULL);

  scaled_size = size * scale;

  /* During a state transition from running to not-running for
   * window-backend apps, it's possible we get a request for the icon.
   * Avoid asserting here and just return an empty image.
   */
  if (app->running_state != NULL)
    window = window_backed_app_get_window (app);

  if (window == NULL)
    {
      ClutterActor *actor;

      actor = clutter_actor_new ();
      g_object_set (actor,
                    "opacity", 0,
                    "width", (float) scaled_size,
                    "height", (float) scaled_size,
                    NULL);
      return actor;
    }

  widget = NULL;

  if (meta_window_get_client_type (window) == META_WINDOW_CLIENT_TYPE_X11)
    {
      cairo_surface_t *icon;

      g_object_get (G_OBJECT (window), "icon", &icon, NULL);

      if (icon != NULL)
        {
          StWidget *texture_actor;

          texture_actor =
            st_texture_cache_bind_cairo_surface_property (st_texture_cache_get_default (),
                                                          G_OBJECT (window),
                                                          "icon",
                                                          scaled_size);

          widget = g_object_new (ST_TYPE_BIN,
                                 "child", texture_actor,
                                 NULL);
        }
    }

  if (widget == NULL)
    {
      widget = g_object_new (ST_TYPE_ICON,
                             "icon-size", size,
                             "icon-type", ST_ICON_FULLCOLOR,
                             "icon-name", "application-x-executable",
                             NULL);
    }
  st_widget_add_style_class_name (widget, "fallback-app-icon");

  return CLUTTER_ACTOR (widget);
}

/**
 * Laminax_app_create_icon_texture:
 *
 * Look up the icon for this application, and create a #ClutterActor
 * for it at the given size.
 *
 * Return value: (transfer none): A floating #ClutterActor
 */
ClutterActor *
Laminax_app_create_icon_texture (LaminaxApp   *app,
                               int         size)
{
  GIcon *icon;
  ClutterActor *ret;

  ret = NULL;

  if (app->info == NULL)
    return window_backed_app_get_icon (app, size);

  icon = g_app_info_get_icon (G_APP_INFO (app->info));

  if (icon != NULL)
    ret = g_object_new (ST_TYPE_ICON, "gicon", icon, "icon-size", size, NULL);

  if (ret == NULL)
    ret = get_failsafe_icon (size);

  return ret;
}


/**
 * Laminax_app_create_icon_texture_for_window:
 * @app: a #LaminaxApp
 * @size: the size of the icon to create
 * @for_window: (nullable): Optional - the backing MetaWindow to look up for.
 *
 * Look up the icon for this application, and create a #ClutterTexture
 * for it at the given size.  If for_window is NULL, it bases the icon
 * off the most-recently-used window for the app, otherwise it attempts to
 * use for_window for determining the icon.
 *
 * Return value: (transfer none): A floating #ClutterActor
 */
ClutterActor *
Laminax_app_create_icon_texture_for_window (LaminaxApp   *app,
                                             int            size,
                                             MetaWindow    *for_window)
{
  MetaWindow *window;

  window = NULL;

  if (app->running_state != NULL)
  {
    const gchar *icon_name;

    if (for_window != NULL)
      {
        if (g_slist_find (app->running_state->windows, for_window) != NULL)
          {
            window = for_window;
          }
        else
          {
            g_warning ("Laminax_app_create_icon_texture: MetaWindow %p provided that does not match App %p",
                       for_window, app);
          }
      }

    if (window != NULL)
      {
        icon_name = meta_window_get_icon_name (window);

        if (icon_name != NULL)
          {
            return get_actor_for_icon_name (app, icon_name, size);
          }
      }
  }

  return Laminax_app_create_icon_texture (app, size);
}

static const char *
get_common_name (LaminaxApp *app)
{
  if (app->entry)
    return g_app_info_get_name (G_APP_INFO (app->info));
  else if (app->running_state == NULL)
    return _("Unknown");
  else
    {
      MetaWindow *window = window_backed_app_get_window (app);
      const char *name = NULL;

      if (window)
        name = meta_window_get_wm_class (window);
      if (!name)
        name = _("Unknown");
      return name;
    }
}

const char *
Laminax_app_get_name (LaminaxApp *app)
{
  if (app->unique_name)
    return app->unique_name;

  return get_common_name (app);
}

const char *
Laminax_app_get_description (LaminaxApp *app)
{
  if (app->entry)
    return g_app_info_get_description (G_APP_INFO (app->info));
  else
    return NULL;
}

const char *
Laminax_app_get_keywords (LaminaxApp *app)
{
  const char * const *keywords;
  const char *keyword;
  gint i;
  gchar *ret = NULL;

  if (app->keywords)
    return app->keywords;

  if (app->info)
    keywords = gmenu_desktopappinfo_get_keywords (app->info);
  else
    keywords = NULL;

  if (keywords != NULL)
    {
      GString *keyword_list = g_string_new(NULL);

      for (i = 0; keywords[i] != NULL; i++)
        {
          keyword = keywords[i];
          g_string_append_printf (keyword_list, "%s;", keyword);
        }

      ret = g_string_free (keyword_list, FALSE);
    }

    app->keywords = ret;

    return ret;
}

gboolean
Laminax_app_get_nodisplay (LaminaxApp *app)
{
  if (app->hidden_as_duplicate)
    {
      return TRUE;
    }

  if (app->entry)
    {
      g_return_val_if_fail (app->info != NULL, TRUE);
      return gmenu_desktopappinfo_get_nodisplay (app->info);
      // return !g_app_info_should_show (G_APP_INFO (app->info));
    }

  return FALSE;
}

/**
 * Laminax_app_is_window_backed:
 *
 * A window backed application is one which represents just an open
 * window, i.e. there's no .desktop file association, so we don't know
 * how to launch it again.
 */
gboolean
Laminax_app_is_window_backed (LaminaxApp *app)
{
  return app->entry == NULL;
}

typedef struct {
  MetaWorkspace *workspace;
  GSList **transients;
} CollectTransientsData;

static gboolean
collect_transients_on_workspace (MetaWindow *window,
                                 gpointer    datap)
{
  CollectTransientsData *data = datap;

  if (data->workspace && meta_window_get_workspace (window) != data->workspace)
    return TRUE;

  *data->transients = g_slist_prepend (*data->transients, window);
  return TRUE;
}

/* The basic idea here is that when we're targeting a window,
 * if it has transients we want to pick the most recent one
 * the user interacted with.
 * This function makes raising GEdit with the file chooser
 * open work correctly.
 */
static MetaWindow *
find_most_recent_transient_on_same_workspace (MetaDisplay *display,
                                              MetaWindow  *reference)
{
  GSList *transients, *transients_sorted, *iter;
  MetaWindow *result;
  CollectTransientsData data;

  transients = NULL;
  data.workspace = meta_window_get_workspace (reference);
  data.transients = &transients;

  meta_window_foreach_transient (reference, collect_transients_on_workspace, &data);

  transients_sorted = meta_display_sort_windows_by_stacking (display, transients);
  /* Reverse this so we're top-to-bottom (yes, we should probably change the order
   * returned from the sort_windows_by_stacking function)
   */
  transients_sorted = g_slist_reverse (transients_sorted);
  g_slist_free (transients);
  transients = NULL;

  result = NULL;
  for (iter = transients_sorted; iter; iter = iter->next)
    {
      MetaWindow *window = iter->data;
      MetaWindowType wintype = meta_window_get_window_type (window);

      /* Don't want to focus UTILITY types, like the Gimp toolbars */
      if (wintype == META_WINDOW_NORMAL ||
          wintype == META_WINDOW_DIALOG)
        {
          result = window;
          break;
        }
    }
  g_slist_free (transients_sorted);
  return result;
}

/**
 * Laminax_app_activate_window:
 * @app: a #LaminaxApp
 * @window: (nullable): Window to be focused
 * @timestamp: Event timestamp
 *
 * Bring all windows for the given app to the foreground,
 * but ensure that @window is on top.  If @window is %NULL,
 * the window with the most recent user time for the app
 * will be used.
 *
 * This function has no effect if @app is not currently running.
 */
void
Laminax_app_activate_window (LaminaxApp     *app,
                           MetaWindow   *window,
                           guint32       timestamp)
{
  GSList *windows;

  if (app->state != Laminax_APP_STATE_RUNNING)
    return;

  windows = Laminax_app_get_windows (app);
  if (window == NULL && windows)
    window = windows->data;

  if (!g_slist_find (windows, window))
    return;
  else
    {
      GSList *iter;
      LaminaxGlobal *global = app->global;
      MetaWorkspaceManager *workspace_manager = global->workspace_manager;
      MetaDisplay *display = global->meta_display;
      MetaWorkspace *active = meta_workspace_manager_get_active_workspace (workspace_manager);
      MetaWorkspace *workspace = meta_window_get_workspace (window);
      guint32 last_user_timestamp = meta_display_get_last_user_time (display);
      MetaWindow *most_recent_transient;

      if (meta_display_xserver_time_is_before (display, timestamp, last_user_timestamp))
        {
          meta_window_set_demands_attention (window);
          return;
        }

      /* Now raise all the other windows for the app that are on
       * the same workspace, in reverse order to preserve the stacking.
       */
      for (iter = windows; iter; iter = iter->next)
        {
          MetaWindow *other_window = iter->data;

          if (other_window != window)
            meta_window_raise (other_window);
        }

      /* If we have a transient that the user's interacted with more recently than
       * the window, pick that.
       */
      most_recent_transient = find_most_recent_transient_on_same_workspace (display, window);
      if (most_recent_transient
          && meta_display_xserver_time_is_before (display,
                                                  meta_window_get_user_time (window),
                                                  meta_window_get_user_time (most_recent_transient)))
        window = most_recent_transient;

      if (active != workspace)
        meta_workspace_activate_with_focus (workspace, window, timestamp);
      else
        meta_window_activate (window, timestamp);
    }
}

/**
 * Laminax_app_activate:
 * @app: a #LaminaxApp
 *
 * Like Laminax_app_activate_full(), but using the default workspace and
 * event timestamp.
 */
void
Laminax_app_activate (LaminaxApp      *app)
{
  return Laminax_app_activate_full (app, -1, 0);
}

/**
 * Laminax_app_activate_full:
 * @app: a #LaminaxApp
 * @workspace: launch on this workspace, or -1 for default. Ignored if
 *   activating an existing window
 * @timestamp: Event timestamp
 *
 * Perform an appropriate default action for operating on this application,
 * dependent on its current state.  For example, if the application is not
 * currently running, launch it.  If it is running, activate the most
 * recently used NORMAL window (or if that window has a transient, the most
 * recently used transient for that window).
 */
void
Laminax_app_activate_full (LaminaxApp      *app,
                         int            workspace,
                         guint32        timestamp)
{
  LaminaxGlobal *global;

  global = app->global;

  if (timestamp == 0)
    timestamp = Laminax_global_get_current_time (global);

  switch (app->state)
    {
      case Laminax_APP_STATE_STOPPED:
        {
          GError *error = NULL;
          if (!Laminax_app_launch (app,
                                 timestamp,
                                 NULL,
                                 workspace,
                                 NULL,
                                 &error))
            {
              char *msg;
              msg = g_strdup_printf (_("Failed to launch '%s'"), Laminax_app_get_name (app));
              Laminax_global_notify_error (global,
                                         msg,
                                         error->message);
              g_free (msg);
              g_clear_error (&error);
            }
        }
        break;
      case Laminax_APP_STATE_STARTING:
        break;
      case Laminax_APP_STATE_RUNNING:
        Laminax_app_activate_window (app, NULL, timestamp);
        break;
      default:
        g_warning("Laminax_app_activate_full: default case");
        break;
    }
}

/**
 * Laminax_app_open_new_window:
 * @app: a #LaminaxApp
 * @workspace: open on this workspace, or -1 for default
 *
 * Request that the application create a new window.
 */
void
Laminax_app_open_new_window (LaminaxApp      *app,
                           int            workspace)
{
  g_return_if_fail (app->entry != NULL);

  /* Here we just always launch the application again, even if we know
   * it was already running.  For most applications this
   * should have the effect of creating a new window, whether that's
   * a second process (in the case of Calculator) or IPC to existing
   * instance (Firefox).  There are a few less-sensical cases such
   * as say Pidgin.  Ideally, we have the application express to us
   * that it supports an explicit new-window action.
   */
  Laminax_app_launch (app,
                    0,
                    NULL,
                    workspace,
                    NULL,
                    NULL);
}

/**
 * Laminax_app_can_open_new_window:
 * @app: a #LaminaxApp
 *
 * Returns %TRUE if the app supports opening a new window through
 * Laminax_app_open_new_window() (ie, if calling that function will
 * result in actually opening a new window and not something else,
 * like presenting the most recently active one)
 */
gboolean
Laminax_app_can_open_new_window (LaminaxApp *app)
{
  /* Apps that are not running can always open new windows, because
     activating them would open the first one */
  if (!app->running_state)
    return TRUE;

  /* If the app doesn't have a desktop file, then nothing is possible */
  if (!app->info)
    return FALSE;

  /* If the app is explicitly telling us, then we know for sure */
  if (gmenu_desktopappinfo_has_key (GMENU_DESKTOPAPPINFO (app->info),
                                  "X-GNOME-SingleWindow"))
    return !gmenu_desktopappinfo_get_boolean (GMENU_DESKTOPAPPINFO (app->info),
                                            "X-GNOME-SingleWindow");

  /* In all other cases, we don't have a reliable source of information
     or a decent heuristic, so we err on the compatibility side and say
     yes.
  */
  return TRUE;
}

/**
 * Laminax_app_get_state:
 * @app: a #LaminaxApp
 *
 * Returns: State of the application
 */
LaminaxAppState
Laminax_app_get_state (LaminaxApp *app)
{
  return app->state;
}

/**
 * Laminax_app_get_is_flatpak:
 * @app: a #LaminaxApp
 *
 * Returns: TRUE if #app is a flatpak app, FALSE if not
 */
gboolean
Laminax_app_get_is_flatpak (LaminaxApp *app)
{
  return app->is_flatpak;
}

typedef struct {
  LaminaxApp *app;
  MetaWorkspace *active_workspace;
} CompareWindowsData;

static int
Laminax_app_compare_windows (gconstpointer   a,
                           gconstpointer   b,
                           gpointer        datap)
{
  MetaWindow *win_a = (gpointer)a;
  MetaWindow *win_b = (gpointer)b;
  CompareWindowsData *data = datap;
  gboolean ws_a, ws_b;
  gboolean vis_a, vis_b;

  ws_a = meta_window_get_workspace (win_a) == data->active_workspace;
  ws_b = meta_window_get_workspace (win_b) == data->active_workspace;

  if (ws_a && !ws_b)
    return -1;
  else if (!ws_a && ws_b)
    return 1;

  vis_a = meta_window_showing_on_its_workspace (win_a);
  vis_b = meta_window_showing_on_its_workspace (win_b);

  if (vis_a && !vis_b)
    return -1;
  else if (!vis_a && vis_b)
    return 1;

  return meta_window_get_user_time (win_b) - meta_window_get_user_time (win_a);
}

/**
 * Laminax_app_get_windows:
 * @app:
 *
 * Get the toplevel, interesting windows which are associated with this
 * application.  The returned list will be sorted first by whether
 * they're on the active workspace, then by whether they're visible,
 * and finally by the time the user last interacted with them.
 *
 * Returns: (transfer none) (element-type MetaWindow): List of windows
 */
GSList *
Laminax_app_get_windows (LaminaxApp *app)
{
  if (app->running_state == NULL)
    return NULL;

  if (app->running_state->window_sort_stale)
    {
      CompareWindowsData data;
      data.app = app;
      data.active_workspace = meta_workspace_manager_get_active_workspace (app->global->workspace_manager);
      app->running_state->windows = g_slist_sort_with_data (app->running_state->windows, Laminax_app_compare_windows, &data);
      app->running_state->window_sort_stale = FALSE;
    }

  return app->running_state->windows;
}

guint
Laminax_app_get_n_windows (LaminaxApp *app)
{
  if (app->running_state == NULL)
    return 0;
  return g_slist_length (app->running_state->windows);
}

gboolean
Laminax_app_is_on_workspace (LaminaxApp *app,
                           MetaWorkspace   *workspace)
{
  GSList *iter;

  if (app->state == Laminax_APP_STATE_STARTING)
    {
      if (app->started_on_workspace == -1 ||
          meta_workspace_index (workspace) == app->started_on_workspace)
        return TRUE;
      else
        return FALSE;
    }

  if (app->running_state == NULL)
    return FALSE;

  for (iter = app->running_state->windows; iter; iter = iter->next)
    {
      if (meta_window_get_workspace (iter->data) == workspace)
        return TRUE;
    }

  return FALSE;
}

LaminaxApp *
_Laminax_app_new_for_window (MetaWindow      *window)
{
  LaminaxApp *app;

  app = g_object_new (Laminax_TYPE_APP, NULL);

  app->window_id_string = g_strdup_printf ("window:%d", meta_window_get_stable_sequence (window));

  _Laminax_app_add_window (app, window);

  return app;
}

LaminaxApp *
_Laminax_app_new (GMenuTreeEntry *info)
{
  LaminaxApp *app;

  app = g_object_new (Laminax_TYPE_APP, NULL);

  _Laminax_app_set_entry (app, info);

  return app;
}

void
_Laminax_app_set_entry (LaminaxApp       *app,
                      GMenuTreeEntry *entry)
{
  g_clear_pointer (&app->entry, gmenu_tree_item_unref);
  g_clear_object (&app->info);

  /* If our entry has changed, our name may have as well, so clear
   * anything set by appsys while deduplicating desktop items. */
  g_clear_pointer (&app->unique_name, g_free);
  app->hidden_as_duplicate = FALSE;

  app->entry = gmenu_tree_item_ref (entry);

  if (entry != NULL)
    {
      app->info = g_object_ref (gmenu_tree_entry_get_app_info (entry));
      app->is_flatpak = app->info && gmenu_desktopappinfo_get_is_flatpak (app->info);
    }
}

static void
Laminax_app_state_transition (LaminaxApp      *app,
                            LaminaxAppState  state)
{
  if (app->state == state)
    return;
  g_return_if_fail (!(app->state == Laminax_APP_STATE_RUNNING &&
                      state == Laminax_APP_STATE_STARTING));
  app->state = state;

  if (app->state == Laminax_APP_STATE_STOPPED && app->running_state)
    {
      unref_running_state (app->running_state);
      app->running_state = NULL;
    }

  _Laminax_app_system_notify_app_state_changed (Laminax_app_system_get_default (), app);

  g_object_notify (G_OBJECT (app), "state");
}

static void
Laminax_app_on_unmanaged (MetaWindow      *window,
                        LaminaxApp *app)
{
  _Laminax_app_remove_window (app, window);
}

static void
Laminax_app_on_ws_switch (MetaWorkspaceManager *workspace_manager,
                        int                 from,
                        int                 to,
                        MetaMotionDirection direction,
                        gpointer            data)
{
  LaminaxApp *app = Laminax_APP (data);

  g_assert (app->running_state != NULL);

  app->running_state->window_sort_stale = TRUE;

  g_signal_emit (app, Laminax_app_signals[WINDOWS_CHANGED], 0);
}

void
_Laminax_app_add_window (LaminaxApp        *app,
                       MetaWindow      *window)
{
  if (app->running_state && g_slist_find (app->running_state->windows, window))
    return;

  g_object_freeze_notify (G_OBJECT (app));

  if (!app->running_state)
      create_running_state (app);

  app->running_state->window_sort_stale = TRUE;
  app->running_state->windows = g_slist_prepend (app->running_state->windows, g_object_ref (window));
  g_signal_connect (window, "unmanaged", G_CALLBACK(Laminax_app_on_unmanaged), app);

  if (app->state != Laminax_APP_STATE_STARTING)
    Laminax_app_state_transition (app, Laminax_APP_STATE_RUNNING);

  g_object_thaw_notify (G_OBJECT (app));

  g_signal_emit (app, Laminax_app_signals[WINDOWS_CHANGED], 0);
}

void
_Laminax_app_remove_window (LaminaxApp   *app,
                          MetaWindow *window)
{
  g_assert (app->running_state != NULL);

  if (!g_slist_find (app->running_state->windows, window))
    return;

  g_signal_handlers_disconnect_by_func (window, G_CALLBACK(Laminax_app_on_unmanaged), app);
  g_object_unref (window);
  app->running_state->windows = g_slist_remove (app->running_state->windows, window);

  if (app->running_state->windows == NULL)
    Laminax_app_state_transition (app, Laminax_APP_STATE_STOPPED);

  g_signal_emit (app, Laminax_app_signals[WINDOWS_CHANGED], 0);
}

/**
 * Laminax_app_get_pids:
 * @app: a #LaminaxApp
 *
 * Returns: (transfer container) (element-type int): An unordered list of process identifiers associated with this application.
 */
GSList *
Laminax_app_get_pids (LaminaxApp *app)
{
  GSList *result;
  GSList *iter;

  result = NULL;
  for (iter = Laminax_app_get_windows (app); iter; iter = iter->next)
    {
      MetaWindow *window = iter->data;
      int pid = meta_window_get_pid (window);
      /* Note in the (by far) common case, app will only have one pid, so
       * we'll hit the first element, so don't worry about O(N^2) here.
       */
      if (!g_slist_find (result, GINT_TO_POINTER (pid)))
        result = g_slist_prepend (result, GINT_TO_POINTER (pid));
    }
  return result;
}

void
_Laminax_app_handle_startup_sequence (LaminaxApp          *app,
                                       MetaStartupSequence  *sequence)
{
  gboolean starting = !meta_startup_sequence_get_completed (sequence);

  /* The Shell design calls for on application launch, the app title
   * appears at top, and no X window is focused.  So when we get
   * a startup-notification for this app, transition it to STARTING
   * if it's currently stopped, set it as our application focus,
   * but focus the no_focus window.
   */
  if (starting && Laminax_app_get_state (app) == Laminax_APP_STATE_STOPPED)
    {
      MetaDisplay *display = Laminax_global_get_display (Laminax_global_get ());

      Laminax_app_state_transition (app, Laminax_APP_STATE_STARTING);
      meta_display_unset_input_focus (display,
                                      meta_startup_sequence_get_timestamp (sequence));
      app->started_on_workspace = meta_startup_sequence_get_workspace (sequence);
    }

  if (!starting)
    {
      if (app->running_state && app->running_state->windows)
        Laminax_app_state_transition (app, Laminax_APP_STATE_RUNNING);
      else /* application have > 1 .desktop file */
        Laminax_app_state_transition (app, Laminax_APP_STATE_STOPPED);
    }
}

const char *
_Laminax_app_get_common_name (LaminaxApp *app)
{
  return get_common_name (app);
}

void
_Laminax_app_set_unique_name (LaminaxApp *app,
                               gchar       *unique_name)
{
  if (app->unique_name)
    {
      g_free (app->unique_name);
    }

  app->unique_name = unique_name;
}

const char *
_Laminax_app_get_unique_name (LaminaxApp *app)
{
  return app->unique_name;
}

const char *
_Laminax_app_get_executable (LaminaxApp *app)
{
  if (app->entry)
    {
      return g_app_info_get_executable (G_APP_INFO (app->info));
    }

  return NULL;
}

const char *
_Laminax_app_get_desktop_path (LaminaxApp *app)
{
  if (app->entry)
    {
      return gmenu_desktopappinfo_get_filename (app->info);
    }

  return NULL;
}

void
_Laminax_app_set_hidden_as_duplicate (LaminaxApp *app,
                                     gboolean     hide)
{
  app->hidden_as_duplicate = hide;
}

/**
 * Laminax_app_request_quit:
 * @app: A #LaminaxApp
 *
 * Initiate an asynchronous request to quit this application.
 * The application may interact with the user, and the user
 * might cancel the quit request from the application UI.
 *
 * This operation may not be supported for all applications.
 *
 * Returns: %TRUE if a quit request is supported for this application
 */
gboolean
Laminax_app_request_quit (LaminaxApp   *app)
{
  LaminaxGlobal *global;
  GSList *iter;

  if (app->state != Laminax_APP_STATE_RUNNING)
    return FALSE;

  /* TODO - check for an XSMP connection; we could probably use that */

  global = app->global;

  for (iter = app->running_state->windows; iter; iter = iter->next)
    {
      MetaWindow *win = iter->data;

      if (!meta_window_can_close (win))
        continue;

      meta_window_delete (win, Laminax_global_get_current_time (global));
    }
  return TRUE;
}

static void
_gather_pid_callback (GDesktopAppInfo   *gapp,
                      GPid               pid,
                      gpointer           data)
{
  LaminaxApp *app;
  LaminaxWindowTracker *tracker;

  g_return_if_fail (data != NULL);

  app = Laminax_APP (data);
  tracker = Laminax_window_tracker_get_default ();

  _Laminax_window_tracker_add_child_process_app (tracker,
                                               pid,
                                               app);
}

static void
apply_discrete_gpu_env (GAppLaunchContext *context)
{
  XAppGpuOffloadHelper *helper = xapp_gpu_offload_helper_get_sync ();
  GList *infos = xapp_gpu_offload_helper_get_offload_infos (helper);

  if (infos != NULL)
    {
      XAppGpuInfo *info = infos->data;
      gchar **env_strv = info->env_strv;

      for (gint i = 0; i < g_strv_length (env_strv); i += 2)
        {
          g_app_launch_context_setenv (context, env_strv[i], env_strv[i + 1]);
        }
    }
}

static gboolean
real_app_launch (LaminaxApp   *app,
                  guint         timestamp,
                  GList        *uris,
                  int           workspace,
                  char        **startup_id,
                  gboolean      offload,
                  GError      **error)
{
  GAppLaunchContext *context;
  gboolean ret;
  LaminaxGlobal *global;
  MetaWorkspaceManager *workspace_manager;

  if (startup_id)
    *startup_id = NULL;

  if (app->entry == NULL)
    {
      MetaWindow *window = window_backed_app_get_window (app);
      /* We can't pass URIs into a window; shouldn't hit this
       * code path.  If we do, fix the caller to disallow it.
       */
      g_return_val_if_fail (uris == NULL, TRUE);

      meta_window_activate (window, timestamp);
      return TRUE;
    }

  global = app->global;
  workspace_manager = global->workspace_manager;

  context = Laminax_global_create_app_launch_context (global);

  if (workspace >= 0)
    {
      MetaWorkspace *ws;
      ws = meta_workspace_manager_get_workspace_by_index (workspace_manager, workspace);
      meta_launch_context_set_workspace (META_LAUNCH_CONTEXT (context), ws);
    }

  GMenuDesktopAppInfo *launch_info;
  GMenuDesktopAppInfo *offload_appinfo = NULL;

  if (offload)
    {
      GKeyFile *keyfile;

      apply_discrete_gpu_env (G_APP_LAUNCH_CONTEXT (context));
      g_debug ("Offloading '%s' to discrete gpu.", Laminax_app_get_name (app));

      /* Desktop files marked DBusActivatable are launched using their GApplication
       * interface. The offload environment variables aren't used in this case. So
       * construct a temporary appinfo via keyfile instead - this disables dbus
       * launching as a side-effect, since that requires the original filename.
       */

      keyfile = g_key_file_new ();
      if (!g_key_file_load_from_file (keyfile,
                                      gmenu_desktopappinfo_get_filename (app->info),
                                      G_KEY_FILE_NONE,
                                      error))
        {
            g_key_file_unref (keyfile);
            g_object_unref (context);
            return FALSE;
        }

      offload_appinfo = gmenu_desktopappinfo_new_from_keyfile (keyfile);
      g_key_file_unref (keyfile);

      launch_info = offload_appinfo;
    }
  else
    {
      launch_info = app->info;
    }

  ret = gmenu_desktopappinfo_launch_uris_as_manager (launch_info, uris,
                                                   G_APP_LAUNCH_CONTEXT (context),
                                                   G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_STDOUT_TO_DEV_NULL  | G_SPAWN_STDERR_TO_DEV_NULL,
                                                   NULL, NULL,
                                                   _gather_pid_callback, app,
                                                   error);
  g_object_unref (context);
  g_clear_object (&offload_appinfo);

  return ret;
}

/**
 * Laminax_app_launch:
 * @timestamp: Event timestamp, or 0 for current event timestamp
 * @uris: (element-type utf8): List of uris to pass to application
 * @workspace: Start on this workspace, or -1 for default
 * @startup_id: (out): Returned startup notification ID, or %NULL if none
 * @error: A #GError
 */
gboolean
Laminax_app_launch (LaminaxApp     *app,
                     guint            timestamp,
                     GList           *uris,
                     int              workspace,
                     char           **startup_id,
                     GError         **error)
{
  GMenuDesktopAppInfo *app_info = Laminax_app_get_app_info(app);

  XAppGpuOffloadHelper *helper = xapp_gpu_offload_helper_get_sync ();

  gboolean wants_offload = (app_info &&
                            gmenu_desktopappinfo_get_boolean(app_info, "PrefersNonDefaultGPU") &&
                            xapp_gpu_offload_helper_is_offload_supported (helper));
  return real_app_launch (app,
                          timestamp,
                          uris,
                          workspace,
                          startup_id,
                          wants_offload,
                          error);
}

/**
 * Laminax_app_launch_offloaded:
 * @timestamp: Event timestamp, or 0 for current event timestamp
 * @uris: (element-type utf8): List of uris to pass to application
 * @workspace: Start on this workspace, or -1 for default
 * @startup_id: (out): Returned startup notification ID, or %NULL if none
 * @error: A #GError
 *
 * Launch an application using the dedicated gpu (if available)
 */
gboolean
Laminax_app_launch_offloaded (LaminaxApp     *app,
                               guint            timestamp,
                               GList           *uris,
                               int              workspace,
                               char           **startup_id,
                               GError         **error)
{
  return real_app_launch (app,
                          timestamp,
                          uris,
                          workspace,
                          startup_id,
                          TRUE,
                          error);
}

/**
 * Laminax_app_get_app_info:
 * @app: a #LaminaxApp
 *
 * Returns: (transfer none): The #GMenuDesktopAppInfo for this app, or %NULL if backed by a window
 */
GMenuDesktopAppInfo *
Laminax_app_get_app_info (LaminaxApp *app)
{
  return app->info;
}

/**
 * Laminax_app_get_tree_entry:
 * @app: a #LaminaxApp
 *
 * Returns: (transfer none): The #GMenuTreeEntry for this app, or %NULL if backed by a window
 */
GMenuTreeEntry *
Laminax_app_get_tree_entry (LaminaxApp *app)
{
  return app->entry;
}

static void
create_running_state (LaminaxApp *app)
{
  MetaWorkspaceManager *workspace_manager;

  g_assert (app->running_state == NULL);

  workspace_manager = app->global->workspace_manager;
  app->running_state = g_slice_new0 (LaminaxAppRunningState);
  app->running_state->refcount = 1;
  app->running_state->workspace_switch_id =
    g_signal_connect (workspace_manager, "workspace-switched", G_CALLBACK(Laminax_app_on_ws_switch), app);
}

static void
unref_running_state (LaminaxAppRunningState *state)
{
  MetaWorkspaceManager *workspace_manager;
  LaminaxGlobal *global;

  state->refcount--;
  if (state->refcount > 0)
    return;

  global = Laminax_global_get ();
  workspace_manager = global->workspace_manager;

  g_signal_handler_disconnect (workspace_manager, state->workspace_switch_id);
  g_slice_free (LaminaxAppRunningState, state);
}

static void
Laminax_app_init (LaminaxApp *self)
{
  self->state = Laminax_APP_STATE_STOPPED;
  self->keywords = NULL;
  self->global = Laminax_global_get ();
}

static void
Laminax_app_dispose (GObject *object)
{
  LaminaxApp *app = Laminax_APP (object);

  if (app->entry)
    {
      gmenu_tree_item_unref (app->entry);
      app->entry = NULL;
    }

  if (app->info)
    {
      g_object_unref (app->info);
      app->info = NULL;
    }

  while (app->running_state)
    _Laminax_app_remove_window (app, app->running_state->windows->data);

  g_clear_pointer (&app->keywords, g_free);
  g_clear_pointer (&app->unique_name, g_free);

  G_OBJECT_CLASS(Laminax_app_parent_class)->dispose (object);
}

static void
Laminax_app_finalize (GObject *object)
{
  LaminaxApp *app = Laminax_APP (object);

  g_free (app->window_id_string);

  G_OBJECT_CLASS(Laminax_app_parent_class)->finalize (object);
}

static void
Laminax_app_class_init(LaminaxAppClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = Laminax_app_get_property;
  gobject_class->dispose = Laminax_app_dispose;
  gobject_class->finalize = Laminax_app_finalize;

  Laminax_app_signals[WINDOWS_CHANGED] = g_signal_new ("windows-changed",
                                     Laminax_TYPE_APP,
                                     G_SIGNAL_RUN_LAST,
                                     0,
                                     NULL, NULL, NULL,
                                     G_TYPE_NONE, 0);

  /**
   * LaminaxApp:state:
   *
   * The high-level state of the application, effectively whether it's
   * running or not, or transitioning between those states.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_STATE,
                                   g_param_spec_enum ("state",
                                                      "State",
                                                      "Application state",
                                                      Laminax_TYPE_APP_STATE,
                                                      Laminax_APP_STATE_STOPPED,
                                                      G_PARAM_READABLE));
}
