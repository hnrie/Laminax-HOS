/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_WINDOW_TRACKER_H__
#define __Laminax_WINDOW_TRACKER_H__

#include <glib-object.h>
#include <glib.h>
#include <meta/window.h>

#include "Laminax-app.h"
#include "Laminax-app-system.h"

G_BEGIN_DECLS

typedef struct _LaminaxWindowTracker LaminaxWindowTracker;
typedef struct _LaminaxWindowTrackerClass LaminaxWindowTrackerClass;
typedef struct _LaminaxWindowTrackerPrivate LaminaxWindowTrackerPrivate;

#define Laminax_TYPE_WINDOW_TRACKER              (Laminax_window_tracker_get_type ())
#define Laminax_WINDOW_TRACKER(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), Laminax_TYPE_WINDOW_TRACKER, LaminaxWindowTracker))
#define Laminax_WINDOW_TRACKER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_WINDOW_TRACKER, LaminaxWindowTrackerClass))
#define Laminax_IS_WINDOW_TRACKER(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), Laminax_TYPE_WINDOW_TRACKER))
#define Laminax_IS_WINDOW_TRACKER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_WINDOW_TRACKER))
#define Laminax_WINDOW_TRACKER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_WINDOW_TRACKER, LaminaxWindowTrackerClass))

struct _LaminaxWindowTrackerClass
{
  GObjectClass parent_class;
};

GType Laminax_window_tracker_get_type (void) G_GNUC_CONST;

LaminaxWindowTracker* Laminax_window_tracker_get_default(void);

LaminaxApp *Laminax_window_tracker_get_window_app (LaminaxWindowTracker *tracker, MetaWindow *metawin);

LaminaxApp *Laminax_window_tracker_get_app_from_pid (LaminaxWindowTracker *tracker, int pid);

gboolean Laminax_window_tracker_is_window_interesting (LaminaxWindowTracker *tracker, MetaWindow *window);

GSList *Laminax_window_tracker_get_startup_sequences (LaminaxWindowTracker *tracker);

G_END_DECLS

#endif /* __Laminax_WINDOW_TRACKER_H__ */
