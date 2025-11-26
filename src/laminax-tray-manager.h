/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#ifndef __Laminax_TRAY_MANAGER_H__
#define __Laminax_TRAY_MANAGER_H__

#include <clutter/clutter.h>
#include "st.h"

G_BEGIN_DECLS

#define Laminax_TYPE_TRAY_MANAGER (Laminax_tray_manager_get_type ())
G_DECLARE_FINAL_TYPE (LaminaxTrayManager, Laminax_tray_manager,
                      Laminax, TRAY_MANAGER, GObject)

LaminaxTrayManager *Laminax_tray_manager_new          (void);
void              Laminax_tray_manager_manage_screen (LaminaxTrayManager *manager,
                                                    StWidget         *theme_widget);
void              Laminax_tray_manager_unmanage_screen (LaminaxTrayManager *manager);
void              Laminax_tray_manager_redisplay (LaminaxTrayManager *manager);
void              Laminax_tray_manager_set_orientation (LaminaxTrayManager *manager,
                                                         ClutterOrientation   orientation);
G_END_DECLS

#endif /* __Laminax_TRAY_MANAGER_H__ */
