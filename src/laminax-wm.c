/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "config.h"

#include <string.h>

#include <meta/meta-enum-types.h>
#include <meta/keybindings.h>

#include "Laminax-wm-private.h"
#include "Laminax-global.h"

struct _LaminaxWM {
  GObject parent;

  MetaPlugin *plugin;
};

/* Signals */
enum
{
  MINIMIZE,
  UNMINIMIZE,
  SIZE_CHANGED,
  SIZE_CHANGE,
  MAP,
  DESTROY,
  SWITCH_WORKSPACE,
  SWITCH_WORKSPACE_COMPLETE,
  KILL_SWITCH_WORKSPACE,
  KILL_WINDOW_EFFECTS,
  SHOW_TILE_PREVIEW,
  HIDE_TILE_PREVIEW,
  SHOW_WINDOW_MENU,
  FILTER_KEYBINDING,
  CONFIRM_DISPLAY_CHANGE,
  CREATE_CLOSE_DIALOG,
  CREATE_INHIBIT_SHORTCUTS_DIALOG,

  LAST_SIGNAL
};

G_DEFINE_TYPE(LaminaxWM, Laminax_wm, G_TYPE_OBJECT);

static guint Laminax_wm_signals [LAST_SIGNAL] = { 0 };

static void
Laminax_wm_init (LaminaxWM *wm)
{
}

static void
Laminax_wm_finalize (GObject *object)
{
  G_OBJECT_CLASS (Laminax_wm_parent_class)->finalize (object);
}

static void
Laminax_wm_class_init (LaminaxWMClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = Laminax_wm_finalize;

  Laminax_wm_signals[MINIMIZE] =
    g_signal_new ("minimize",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  Laminax_wm_signals[UNMINIMIZE] =
    g_signal_new ("unminimize",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  Laminax_wm_signals[SIZE_CHANGED] =
    g_signal_new ("size-changed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  Laminax_wm_signals[SIZE_CHANGE] =
    g_signal_new ("size-change",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 4,
                  META_TYPE_WINDOW_ACTOR, META_TYPE_SIZE_CHANGE, META_TYPE_RECTANGLE, META_TYPE_RECTANGLE);
  Laminax_wm_signals[MAP] =
    g_signal_new ("map",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  Laminax_wm_signals[DESTROY] =
    g_signal_new ("destroy",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1,
                  META_TYPE_WINDOW_ACTOR);
  Laminax_wm_signals[SWITCH_WORKSPACE] =
    g_signal_new ("switch-workspace",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 3,
                  G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
  Laminax_wm_signals[KILL_SWITCH_WORKSPACE] =
    g_signal_new ("kill-switch-workspace",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL, NULL,
		  G_TYPE_NONE, 0);
  Laminax_wm_signals[SWITCH_WORKSPACE_COMPLETE] =
    g_signal_new ("switch-workspace-complete",
          G_TYPE_FROM_CLASS (klass),
          G_SIGNAL_RUN_LAST,
          0,
          NULL, NULL, NULL,
          G_TYPE_NONE, 0);
  Laminax_wm_signals[KILL_WINDOW_EFFECTS] =
    g_signal_new ("kill-window-effects",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL, NULL,
		  G_TYPE_NONE, 1,
		  META_TYPE_WINDOW_ACTOR);
    Laminax_wm_signals[SHOW_TILE_PREVIEW] =
        g_signal_new ("show-tile-preview",
                     G_TYPE_FROM_CLASS (klass),
                     G_SIGNAL_RUN_LAST,
                     0, NULL, NULL, NULL,
                     G_TYPE_NONE, 3,
                     META_TYPE_WINDOW,
                     META_TYPE_RECTANGLE,
                     G_TYPE_INT);
    Laminax_wm_signals[HIDE_TILE_PREVIEW] =
        g_signal_new ("hide-tile-preview",
                     G_TYPE_FROM_CLASS (klass),
                     G_SIGNAL_RUN_LAST,
                     0,
                     NULL, NULL, NULL,
                     G_TYPE_NONE, 0);
    Laminax_wm_signals[SHOW_WINDOW_MENU] =
        g_signal_new ("show-window-menu",
                     G_TYPE_FROM_CLASS (klass),
                     G_SIGNAL_RUN_LAST,
                     0, NULL, NULL, NULL,
                     G_TYPE_NONE, 3,
                     META_TYPE_WINDOW, G_TYPE_INT, META_TYPE_RECTANGLE);
    Laminax_wm_signals[FILTER_KEYBINDING] =
        g_signal_new ("filter-keybinding",
                     G_TYPE_FROM_CLASS (klass),
                     G_SIGNAL_RUN_LAST,
                     0,
                     g_signal_accumulator_true_handled, NULL, NULL,
                     G_TYPE_BOOLEAN, 1,
                     META_TYPE_KEY_BINDING);
    Laminax_wm_signals[CONFIRM_DISPLAY_CHANGE] =
    g_signal_new ("confirm-display-change",
                     G_TYPE_FROM_CLASS (klass),
                     G_SIGNAL_RUN_LAST,
                     0,
                     NULL, NULL, NULL,
                     G_TYPE_NONE, 0);
  /**
   * LaminaxWM::create-close-dialog:
   * @wm: The WM
   * @window: The window to create the dialog for
   *
   * Creates a close dialog for the given window.
   *
   * Returns: (transfer full): The close dialog instance.
   */
  Laminax_wm_signals[CREATE_CLOSE_DIALOG] =
    g_signal_new ("create-close-dialog",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  META_TYPE_CLOSE_DIALOG, 1, META_TYPE_WINDOW);
  /**
   * LaminaxWM::create-inhibit-shortcuts-dialog:
   * @wm: The WM
   * @window: The window to create the dialog for
   *
   * Creates an inhibit shortcuts dialog for the given window.
   *
   * Returns: (transfer full): The inhibit shortcuts dialog instance.
   */
  Laminax_wm_signals[CREATE_INHIBIT_SHORTCUTS_DIALOG] =
    g_signal_new ("create-inhibit-shortcuts-dialog",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  META_TYPE_INHIBIT_SHORTCUTS_DIALOG, 1, META_TYPE_WINDOW);
}

void
_Laminax_wm_switch_workspace (LaminaxWM      *wm,
                            gint          from,
                            gint          to,
                            MetaMotionDirection direction)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[SWITCH_WORKSPACE], 0,
                 from, to, direction);
}

/**
 * Laminax_wm_completed_switch_workspace:
 * @wm: the LaminaxWM
 *
 * The plugin must call this when it has finished switching the
 * workspace.
 **/
void
Laminax_wm_completed_switch_workspace (LaminaxWM *wm)
{
  g_debug ("%s", G_STRFUNC);
  meta_plugin_switch_workspace_completed (wm->plugin);
  g_signal_emit (wm, Laminax_wm_signals[SWITCH_WORKSPACE_COMPLETE], 0);
}

/**
 * Laminax_wm_completed_minimize:
 * @wm: the LaminaxWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window minimize effect.
 **/
void
Laminax_wm_completed_minimize (LaminaxWM         *wm,
                             MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  meta_plugin_minimize_completed (wm->plugin, actor);
}

/**
 * Laminax_wm_completed_unminimize:
 * @wm: the LaminaxWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window unminimize effect.
 **/
void
Laminax_wm_completed_unminimize (LaminaxWM         *wm,
                             MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  meta_plugin_unminimize_completed (wm->plugin, actor);
}

void
Laminax_wm_completed_size_change  (LaminaxWM         *wm,
                             MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  meta_plugin_size_change_completed (wm->plugin, actor);
}

/**
 * Laminax_wm_completed_map:
 * @wm: the LaminaxWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window map effect.
 **/
void
Laminax_wm_completed_map (LaminaxWM         *wm,
                        MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  meta_plugin_map_completed (wm->plugin, actor);
}

/**
 * Laminax_wm_completed_destroy:
 * @wm: the LaminaxWM
 * @actor: the MetaWindowActor actor
 *
 * The plugin must call this when it has completed a window destroy effect.
 **/
void
Laminax_wm_completed_destroy (LaminaxWM         *wm,
                            MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  meta_plugin_destroy_completed (wm->plugin, actor);
}

/**
 * Laminax_wm_complete_display_change:
 * @wm: the LaminaxWM
 * @ok: if the new configuration was OK
 *
 * The plugin must call this after the user responded to the confirmation dialog.
 */
void
Laminax_wm_complete_display_change (LaminaxWM  *wm,
                                     gboolean  ok)
{
  g_debug ("%s", G_STRFUNC);
  meta_plugin_complete_display_change (wm->plugin, ok);
}

void
_Laminax_wm_kill_switch_workspace (LaminaxWM      *wm)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[KILL_SWITCH_WORKSPACE], 0);
}

void
_Laminax_wm_kill_window_effects (LaminaxWM         *wm,
                               MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[KILL_WINDOW_EFFECTS], 0, actor);
}

void
_Laminax_wm_show_tile_preview (LaminaxWM      *wm,
                                MetaWindow      *window,
                                MetaRectangle   *tile_rect,
                                int            tile_monitor)
{
  g_debug ("%s", G_STRFUNC);
    g_signal_emit (wm, Laminax_wm_signals[SHOW_TILE_PREVIEW], 0,
                   window, tile_rect, tile_monitor);
}

void
_Laminax_wm_hide_tile_preview (LaminaxWM *wm)
{
  g_debug ("%s", G_STRFUNC);
    g_signal_emit (wm, Laminax_wm_signals[HIDE_TILE_PREVIEW], 0);
}

void
_Laminax_wm_show_window_menu (LaminaxWM            *wm,
                            MetaWindow         *window,
                            MetaWindowMenuType  menu,
                            int                 x,
                            int                 y)
{
  g_debug ("%s", G_STRFUNC);
  MetaRectangle rect;

  rect.x = x;
  rect.y = y;
  rect.width = rect.height = 0;

  _Laminax_wm_show_window_menu_for_rect (wm, window, menu, &rect);
}

void
_Laminax_wm_show_window_menu_for_rect (LaminaxWM            *wm,
                                     MetaWindow         *window,
                                     MetaWindowMenuType  menu,
                                     MetaRectangle      *rect)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[SHOW_WINDOW_MENU], 0, window, menu, rect);
}

void
_Laminax_wm_minimize (LaminaxWM         *wm,
                    MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[MINIMIZE], 0, actor);
}

void
_Laminax_wm_unminimize (LaminaxWM         *wm,
                      MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[UNMINIMIZE], 0, actor);
}

void
_Laminax_wm_size_changed (LaminaxWM         *wm,
                        MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[SIZE_CHANGED], 0, actor);
}

void
_Laminax_wm_size_change (LaminaxWM         *wm,
                   MetaWindowActor    *actor,
                       MetaSizeChange   which_change,
                       MetaRectangle   *old_frame_rect,
                       MetaRectangle   *old_buffer_rect)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[SIZE_CHANGE], 0, actor, which_change, old_frame_rect, old_buffer_rect);
}

void
_Laminax_wm_map (LaminaxWM         *wm,
               MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[MAP], 0, actor);
}

void
_Laminax_wm_destroy (LaminaxWM         *wm,
                   MetaWindowActor *actor)
{
  g_debug ("%s", G_STRFUNC);
  g_signal_emit (wm, Laminax_wm_signals[DESTROY], 0, actor);
}

gboolean
_Laminax_wm_filter_keybinding (LaminaxWM          *wm,
                                MetaKeyBinding      *binding)
{
  gboolean rv;

  g_signal_emit (wm, Laminax_wm_signals[FILTER_KEYBINDING], 0, binding, &rv);

  return rv;
}

void
_Laminax_wm_confirm_display_change (LaminaxWM *wm)
{
  g_signal_emit (wm, Laminax_wm_signals[CONFIRM_DISPLAY_CHANGE], 0);
}

MetaCloseDialog *
_Laminax_wm_create_close_dialog (LaminaxWM    *wm,
                                  MetaWindow *window)
{
  MetaCloseDialog *dialog;

  g_signal_emit (wm, Laminax_wm_signals[CREATE_CLOSE_DIALOG], 0, window, &dialog);

  return dialog;
}

MetaInhibitShortcutsDialog *
_Laminax_wm_create_inhibit_shortcuts_dialog (LaminaxWM    *wm,
                                              MetaWindow *window)
{
  MetaInhibitShortcutsDialog *dialog;

  g_signal_emit (wm, Laminax_wm_signals[CREATE_INHIBIT_SHORTCUTS_DIALOG], 0, window, &dialog);

  return dialog;
}
/**
 * Laminax_wm_new:
 * @plugin: the #MetaPlugin
 *
 * Creates a new window management interface by hooking into @plugin.
 *
 * Return value: the new window-management interface
 **/
LaminaxWM *
Laminax_wm_new (MetaPlugin *plugin)
{
  LaminaxWM *wm;

  wm = g_object_new (Laminax_TYPE_WM, NULL);
  wm->plugin = plugin;

  return wm;
}

