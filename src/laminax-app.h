/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_APP_H__
#define __Laminax_APP_H__

#include <clutter/clutter.h>
#include <gio/gio.h>
#include <meta/window.h>
#include <meta/meta-startup-notification.h>

#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-tree.h>
#include <gmenu-desktopappinfo.h>

G_BEGIN_DECLS

typedef struct _LaminaxApp LaminaxApp;
typedef struct _LaminaxAppClass LaminaxAppClass;
typedef struct _LaminaxAppPrivate LaminaxAppPrivate;

#define Laminax_TYPE_APP              (Laminax_app_get_type ())
#define Laminax_APP(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), Laminax_TYPE_APP, LaminaxApp))
#define Laminax_APP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_APP, LaminaxAppClass))
#define Laminax_IS_APP(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), Laminax_TYPE_APP))
#define Laminax_IS_APP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_APP))
#define Laminax_APP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_APP, LaminaxAppClass))

struct _LaminaxAppClass
{
  GObjectClass parent_class;

};

typedef enum {
  Laminax_APP_STATE_STOPPED,
  Laminax_APP_STATE_STARTING,
  Laminax_APP_STATE_RUNNING
} LaminaxAppState;

GType Laminax_app_get_type (void) G_GNUC_CONST;

const char *Laminax_app_get_id (LaminaxApp *app);
GMenuTreeEntry *Laminax_app_get_tree_entry (LaminaxApp *app);
GMenuDesktopAppInfo *Laminax_app_get_app_info (LaminaxApp *app);
ClutterActor *Laminax_app_create_icon_texture (LaminaxApp *app,
                                                int          size);
ClutterActor *Laminax_app_create_icon_texture_for_window (LaminaxApp   *app,
                                                           int            size,
                                                           MetaWindow    *for_window);
const char *Laminax_app_get_name (LaminaxApp *app);
const char *Laminax_app_get_description (LaminaxApp *app);
const char *Laminax_app_get_keywords (LaminaxApp *app);
gboolean Laminax_app_get_nodisplay (LaminaxApp *app);

gboolean Laminax_app_is_window_backed (LaminaxApp *app);

void Laminax_app_activate_window (LaminaxApp *app, MetaWindow *window, guint32 timestamp);

void Laminax_app_activate (LaminaxApp      *app);

void Laminax_app_activate_full (LaminaxApp      *app,
                              int            workspace,
                              guint32        timestamp);

void Laminax_app_open_new_window (LaminaxApp *app,
                                int       workspace);

gboolean Laminax_app_can_open_new_window (LaminaxApp *app);

LaminaxAppState Laminax_app_get_state (LaminaxApp *app);

gboolean Laminax_app_request_quit (LaminaxApp *app);

guint Laminax_app_get_n_windows (LaminaxApp *app);

GSList *Laminax_app_get_windows (LaminaxApp *app);

GSList *Laminax_app_get_pids (LaminaxApp *app);

gboolean Laminax_app_is_on_workspace (LaminaxApp *app, MetaWorkspace *workspace);

gboolean Laminax_app_launch (LaminaxApp     *app,
                           guint         timestamp,
                           GList        *uris,
                           int           workspace,
                           char        **startup_id,
                           GError      **error);

gboolean Laminax_app_launch_offloaded (LaminaxApp     *app,
                           guint         timestamp,
                           GList        *uris,
                           int           workspace,
                           char        **startup_id,
                           GError      **error);

gboolean Laminax_app_get_is_flatpak (LaminaxApp *app);
char * Laminax_app_get_flatpak_app_id (LaminaxApp *app);

G_END_DECLS

#endif /* __Laminax_APP_H__ */
