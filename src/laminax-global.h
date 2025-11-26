/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_GLOBAL_H__
#define __Laminax_GLOBAL_H__

#include <clutter/clutter.h>
#include <glib-object.h>
#include <meta/meta-plugin.h>
#include "Laminax-screen.h"

G_BEGIN_DECLS

typedef struct _LaminaxGlobal      LaminaxGlobal;
typedef struct _LaminaxGlobalClass LaminaxGlobalClass;

#define Laminax_TYPE_GLOBAL              (Laminax_global_get_type ())
#define Laminax_GLOBAL(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), Laminax_TYPE_GLOBAL, LaminaxGlobal))
#define Laminax_GLOBAL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_GLOBAL, LaminaxGlobalClass))
#define Laminax_IS_GLOBAL(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), Laminax_TYPE_GLOBAL))
#define Laminax_IS_GLOBAL_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_GLOBAL))
#define Laminax_GLOBAL_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_GLOBAL, LaminaxGlobalClass))

struct _LaminaxGlobalClass
{
  GObjectClass parent_class;
};

GType Laminax_global_get_type (void) G_GNUC_CONST;

LaminaxGlobal   *Laminax_global_get                       (void);

ClutterStage  *Laminax_global_get_stage                 (LaminaxGlobal *global);
LaminaxScreen *Laminax_global_get_screen                (LaminaxGlobal *global);
MetaDisplay   *Laminax_global_get_display               (LaminaxGlobal *global);
GList         *Laminax_global_get_window_actors         (LaminaxGlobal *global);
GSettings     *Laminax_global_get_settings              (LaminaxGlobal *global);
guint32        Laminax_global_get_current_time          (LaminaxGlobal *global);
pid_t          Laminax_global_get_pid                   (LaminaxGlobal *global);
gchar         *Laminax_global_get_md5_for_string        (LaminaxGlobal *global, const gchar *string);
void           Laminax_global_dump_gjs_stack            (LaminaxGlobal *global);

/* Input/event handling */
gboolean Laminax_global_begin_modal            (LaminaxGlobal         *global,
                                              guint32              timestamp,
                                              MetaModalOptions    options);
void     Laminax_global_end_modal              (LaminaxGlobal         *global,
                                              guint32              timestamp);

typedef enum {
  Laminax_STAGE_INPUT_MODE_NONREACTIVE,
  Laminax_STAGE_INPUT_MODE_NORMAL,
  Laminax_STAGE_INPUT_MODE_FOCUSED,
  Laminax_STAGE_INPUT_MODE_FULLSCREEN
} LaminaxStageInputMode;

void     Laminax_global_set_stage_input_mode   (LaminaxGlobal         *global,
                                              LaminaxStageInputMode  mode);
void     Laminax_global_set_stage_input_region (LaminaxGlobal         *global,
                                              GSList              *rectangles);

/* X utilities */
typedef enum {
  Laminax_CURSOR_DND_IN_DRAG,
  Laminax_CURSOR_DND_UNSUPPORTED_TARGET,
  Laminax_CURSOR_DND_MOVE,
  Laminax_CURSOR_DND_COPY,
  Laminax_CURSOR_POINTING_HAND,
  Laminax_CURSOR_RESIZE_BOTTOM,
  Laminax_CURSOR_RESIZE_TOP,
  Laminax_CURSOR_RESIZE_LEFT,
  Laminax_CURSOR_RESIZE_RIGHT,
  Laminax_CURSOR_RESIZE_BOTTOM_RIGHT,
  Laminax_CURSOR_RESIZE_BOTTOM_LEFT,
  Laminax_CURSOR_RESIZE_TOP_RIGHT,
  Laminax_CURSOR_RESIZE_TOP_LEFT,
  Laminax_CURSOR_CROSSHAIR,
  Laminax_CURSOR_TEXT
} LaminaxCursor;

void    Laminax_global_set_cursor              (LaminaxGlobal         *global,
                                              LaminaxCursor          type);
void    Laminax_global_unset_cursor            (LaminaxGlobal         *global);

guint32 Laminax_global_create_pointer_barrier  (LaminaxGlobal         *global,
                                              int                  x1,
                                              int                  y1,
                                              int                  x2,
                                              int                  y2,
                                              int                  directions);
void    Laminax_global_destroy_pointer_barrier (LaminaxGlobal         *global,
                                              guint32              barrier);

void    Laminax_global_get_pointer             (LaminaxGlobal         *global,
                                              int                 *x,
                                              int                 *y,
                                              ClutterModifierType *mods);
void    Laminax_global_set_pointer             (LaminaxGlobal         *global,
                                              int                 x,
                                              int                 y);

/* Run-at-leisure API */
void Laminax_global_begin_work     (LaminaxGlobal          *global);
void Laminax_global_end_work       (LaminaxGlobal          *global);

typedef void (*LaminaxLeisureFunction) (gpointer data);

void Laminax_global_run_at_leisure (LaminaxGlobal          *global,
                                  LaminaxLeisureFunction  func,
                                  gpointer              user_data,
                                  GDestroyNotify        notify);


/* Misc utilities / Laminax API */

void     Laminax_global_sync_pointer              (LaminaxGlobal  *global);

GAppLaunchContext *
         Laminax_global_create_app_launch_context (LaminaxGlobal  *global);

void     Laminax_global_notify_error              (LaminaxGlobal  *global,
                                                 const char   *msg,
                                                 const char   *details);

void     Laminax_global_real_restart              (LaminaxGlobal  *global);
void     Laminax_global_reexec_self               (LaminaxGlobal  *global);

void     Laminax_global_segfault                  (LaminaxGlobal  *global);
void     Laminax_global_alloc_leak                (LaminaxGlobal  *global,
                                                    gint             mb);

G_END_DECLS

#endif /* __Laminax_GLOBAL_H__ */
