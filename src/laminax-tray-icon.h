/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_TRAY_ICON_H__
#define __Laminax_TRAY_ICON_H__

#include "Laminax-gtk-embed.h"

#define Laminax_TYPE_TRAY_ICON                 (Laminax_tray_icon_get_type ())
#define Laminax_TRAY_ICON(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), Laminax_TYPE_TRAY_ICON, LaminaxTrayIcon))
#define Laminax_TRAY_ICON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_TRAY_ICON, LaminaxTrayIconClass))
#define Laminax_IS_TRAY_ICON(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), Laminax_TYPE_TRAY_ICON))
#define Laminax_IS_TRAY_ICON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_TRAY_ICON))
#define Laminax_TRAY_ICON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_TRAY_ICON, LaminaxTrayIconClass))

typedef struct _LaminaxTrayIcon        LaminaxTrayIcon;
typedef struct _LaminaxTrayIconClass   LaminaxTrayIconClass;
typedef struct _LaminaxTrayIconPrivate LaminaxTrayIconPrivate;

struct _LaminaxTrayIcon
{
    LaminaxGtkEmbed parent;

    LaminaxTrayIconPrivate *priv;
};

struct _LaminaxTrayIconClass
{
    LaminaxGtkEmbedClass parent_class;
};


GType         Laminax_tray_icon_get_type (void) G_GNUC_CONST;
ClutterActor *Laminax_tray_icon_new      (LaminaxEmbeddedWindow *window);

gboolean      Laminax_tray_icon_handle_event (LaminaxTrayIcon *icon,
                                               ClutterEventType  event_type,
                                               ClutterEvent     *event);

#endif /* __Laminax_TRAY_ICON_H__ */
