/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_WM_PRIVATE_H__
#define __Laminax_WM_PRIVATE_H__

#include "Laminax-wm.h"

G_BEGIN_DECLS

/* These forward along the different effects from LaminaxPlugin */

void _Laminax_wm_minimize   (LaminaxWM         *wm,
                           MetaWindowActor *actor);
void _Laminax_wm_unminimize (LaminaxWM         *wm,
                           MetaWindowActor *actor);
void _Laminax_wm_size_changed(LaminaxWM         *wm,
                            MetaWindowActor *actor);
void _Laminax_wm_size_change(LaminaxWM         *wm,
                           MetaWindowActor *actor,
                           MetaSizeChange   which_change,
                           MetaRectangle   *old_frame_rect,
                           MetaRectangle   *old_buffer_rect);
void _Laminax_wm_map        (LaminaxWM         *wm,
                           MetaWindowActor *actor);
void _Laminax_wm_destroy    (LaminaxWM         *wm,
                           MetaWindowActor *actor);

void _Laminax_wm_switch_workspace      (LaminaxWM             *wm,
                                      gint                 from,
                                      gint                 to,
                                      MetaMotionDirection  direction);
void _Laminax_wm_kill_window_effects   (LaminaxWM             *wm,
                                      MetaWindowActor     *actor);
void _Laminax_wm_kill_switch_workspace (LaminaxWM             *wm);

void _Laminax_wm_show_tile_preview     (LaminaxWM         *wm,
                                         MetaWindow         *window,
                                         MetaRectangle      *tile_rect,
                                      int                  tile_monitor);
void _Laminax_wm_hide_tile_preview     (LaminaxWM         *wm);
void _Laminax_wm_show_window_menu      (LaminaxWM             *wm,
                                      MetaWindow          *window,
                                      MetaWindowMenuType   menu,
                                      int                  x,
                                      int                  y);
void _Laminax_wm_show_window_menu_for_rect (LaminaxWM             *wm,
                                          MetaWindow          *window,
                                          MetaWindowMenuType   menu,
                                          MetaRectangle       *rect);

gboolean _Laminax_wm_filter_keybinding (LaminaxWM             *wm,
                                      MetaKeyBinding      *binding);

void _Laminax_wm_confirm_display_change (LaminaxWM            *wm);

MetaCloseDialog * _Laminax_wm_create_close_dialog (LaminaxWM     *wm,
                                                 MetaWindow  *window);

MetaInhibitShortcutsDialog * _Laminax_wm_create_inhibit_shortcuts_dialog (LaminaxWM     *wm,
                                                                        MetaWindow  *window);

G_END_DECLS

#endif /* __Laminax_WM_PRIVATE_H__ */
