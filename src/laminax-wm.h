/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_WM_H__
#define __Laminax_WM_H__

#include <glib-object.h>
#include <meta/meta-plugin.h>

G_BEGIN_DECLS


#define Laminax_TYPE_WM              (Laminax_wm_get_type ())
G_DECLARE_FINAL_TYPE (LaminaxWM, Laminax_wm, Laminax, WM, GObject)

LaminaxWM *Laminax_wm_new                        (MetaPlugin      *plugin);

void     Laminax_wm_completed_minimize         (LaminaxWM         *wm,
                                              MetaWindowActor *actor);
void     Laminax_wm_completed_unminimize       (LaminaxWM         *wm,
                                              MetaWindowActor *actor);
void     Laminax_wm_completed_size_change      (LaminaxWM         *wm,
                                              MetaWindowActor *actor);
void     Laminax_wm_completed_map              (LaminaxWM         *wm,
                                              MetaWindowActor *actor);
void     Laminax_wm_completed_destroy          (LaminaxWM         *wm,
                                              MetaWindowActor *actor);
void     Laminax_wm_completed_switch_workspace (LaminaxWM         *wm);
void     Laminax_wm_complete_display_change    (LaminaxWM         *wm,
                                              gboolean         ok);

G_END_DECLS

#endif /* __Laminax_WM_H__ */
