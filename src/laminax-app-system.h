/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_APP_SYSTEM_H__
#define __Laminax_APP_SYSTEM_H__

#include <gio/gio.h>
#include <clutter/clutter.h>
#include <meta/window.h>
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-tree.h>

#include "Laminax-app.h"

#define Laminax_TYPE_APP_SYSTEM                 (Laminax_app_system_get_type ())
#define Laminax_APP_SYSTEM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), Laminax_TYPE_APP_SYSTEM, LaminaxAppSystem))
#define Laminax_APP_SYSTEM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_APP_SYSTEM, LaminaxAppSystemClass))
#define Laminax_IS_APP_SYSTEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), Laminax_TYPE_APP_SYSTEM))
#define Laminax_IS_APP_SYSTEM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_APP_SYSTEM))
#define Laminax_APP_SYSTEM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_APP_SYSTEM, LaminaxAppSystemClass))

typedef struct _LaminaxAppSystem LaminaxAppSystem;
typedef struct _LaminaxAppSystemClass LaminaxAppSystemClass;
typedef struct _LaminaxAppSystemPrivate LaminaxAppSystemPrivate;

struct _LaminaxAppSystem
{
  GObject parent;

  LaminaxAppSystemPrivate *priv;
};

struct _LaminaxAppSystemClass
{
  GObjectClass parent_class;

  void (*installed_changed)(LaminaxAppSystem *appsys, gpointer user_data);
  void (*favorites_changed)(LaminaxAppSystem *appsys, gpointer user_data);
};

GType           Laminax_app_system_get_type    (void) G_GNUC_CONST;
LaminaxAppSystem *Laminax_app_system_get_default (void);

GMenuTree      *Laminax_app_system_get_tree                     (LaminaxAppSystem *system);

LaminaxApp       *Laminax_app_system_lookup_app                   (LaminaxAppSystem  *system,
                                                               const char      *id);
LaminaxApp       *Laminax_app_system_lookup_startup_wmclass       (LaminaxAppSystem *system,
                                                                     const char     *wmclass);
LaminaxApp       *Laminax_app_system_lookup_desktop_wmclass       (LaminaxAppSystem *system,
                                                                     const char     *wmclass);
LaminaxApp       *Laminax_app_system_lookup_flatpak_app_id (LaminaxAppSystem *system,
                                                              const char        *app_id);

GSList         *Laminax_app_system_get_all                   (LaminaxAppSystem  *system);

GSList         *Laminax_app_system_get_running               (LaminaxAppSystem  *self);

#endif /* __Laminax_APP_SYSTEM_H__ */
