/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_RECORDER_H__
#define __Laminax_RECORDER_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

/**
 * SECTION:LaminaxRecorder
 * short_description: Record from a #ClutterStage
 *
 * The #LaminaxRecorder object is used to make recordings ("screencasts")
 * of a #ClutterStage. Recording is done via #GStreamer. The default is
 * to encode as a Theora movie and write it to a file in the current
 * directory named after the date, but the encoding and output can
 * be configured.
 */
typedef struct _LaminaxRecorder      LaminaxRecorder;
typedef struct _LaminaxRecorderClass LaminaxRecorderClass;

#define Laminax_TYPE_RECORDER              (Laminax_recorder_get_type ())
#define Laminax_RECORDER(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), Laminax_TYPE_RECORDER, LaminaxRecorder))
#define Laminax_RECORDER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_RECORDER, LaminaxRecorderClass))
#define Laminax_IS_RECORDER(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), Laminax_TYPE_RECORDER))
#define Laminax_IS_RECORDER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_RECORDER))
#define Laminax_RECORDER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_RECORDER, LaminaxRecorderClass))

GType              Laminax_recorder_get_type     (void) G_GNUC_CONST;

LaminaxRecorder     *Laminax_recorder_new (ClutterStage  *stage);

void               Laminax_recorder_set_framerate (LaminaxRecorder *recorder,
                                                    int framerate);
void               Laminax_recorder_set_file_template (LaminaxRecorder *recorder,
                                                        const char    *file_template);
void               Laminax_recorder_set_pipeline (LaminaxRecorder *recorder,
                                                   const char    *pipeline);
void               Laminax_recorder_set_draw_cursor (LaminaxRecorder *recorder,
                                                      gboolean       draw_cursor);
void Laminax_recorder_set_area (LaminaxRecorder *recorder,
                                 int               x,
                                 int               y,
                                 int               width,
                                 int               height);
gboolean           Laminax_recorder_record       (LaminaxRecorder  *recorder,
                                                   char          **filename_used);
void               Laminax_recorder_close        (LaminaxRecorder *recorder);
gboolean           Laminax_recorder_is_recording (LaminaxRecorder *recorder);

G_END_DECLS

#endif /* __Laminax_RECORDER_H__ */
