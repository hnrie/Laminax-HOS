/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_PERF_LOG_H__
#define __Laminax_PERF_LOG_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _LaminaxPerfLog LaminaxPerfLog;
typedef struct _LaminaxPerfLogClass LaminaxPerfLogClass;

#define Laminax_TYPE_PERF_LOG              (Laminax_perf_log_get_type ())
#define Laminax_PERF_LOG(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), Laminax_TYPE_PERF_LOG, LaminaxPerfLog))
#define Laminax_PERF_LOG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_PERF_LOG, LaminaxPerfLogClass))
#define Laminax_IS_PERF_LOG(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), Laminax_TYPE_PERF_LOG))
#define Laminax_IS_PERF_LOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_PERF_LOG))
#define Laminax_PERF_LOG_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_PERF_LOG, LaminaxPerfLogClass))

GType Laminax_perf_log_get_type (void) G_GNUC_CONST;

LaminaxPerfLog *Laminax_perf_log_get_default (void);

void Laminax_perf_log_set_enabled (LaminaxPerfLog *perf_log,
				 gboolean      enabled);

void Laminax_perf_log_define_event (LaminaxPerfLog *perf_log,
				  const char   *name,
				  const char   *description,
				  const char   *signature);
void Laminax_perf_log_event        (LaminaxPerfLog *perf_log,
				  const char   *name);
void Laminax_perf_log_event_i      (LaminaxPerfLog *perf_log,
				  const char   *name,
				  gint32        arg);
void Laminax_perf_log_event_x      (LaminaxPerfLog *perf_log,
				  const char   *name,
				  gint64        arg);
void Laminax_perf_log_event_s      (LaminaxPerfLog *perf_log,
				  const char   *name,
				  const char   *arg);

void Laminax_perf_log_define_statistic (LaminaxPerfLog *perf_log,
                                      const char   *name,
                                      const char   *description,
                                      const char   *signature);

void Laminax_perf_log_update_statistic_i (LaminaxPerfLog *perf_log,
                                        const char   *name,
                                        int           value);
void Laminax_perf_log_update_statistic_x (LaminaxPerfLog *perf_log,
                                        const char   *name,
                                        gint64        value);

typedef void (*LaminaxPerfStatisticsCallback) (LaminaxPerfLog *perf_log,
                                             gpointer      data);

void Laminax_perf_log_add_statistics_callback (LaminaxPerfLog               *perf_log,
                                             LaminaxPerfStatisticsCallback callback,
                                             gpointer                    user_data,
                                             GDestroyNotify              notify);

void Laminax_perf_log_collect_statistics (LaminaxPerfLog *perf_log);

typedef void (*LaminaxPerfReplayFunction) (gint64      time,
					 const char *name,
					 const char *signature,
					 GValue     *arg,
                                         gpointer    user_data);

void Laminax_perf_log_replay (LaminaxPerfLog            *perf_log,
			    LaminaxPerfReplayFunction  replay_function,
                            gpointer                 user_data);

gboolean Laminax_perf_log_dump_events (LaminaxPerfLog   *perf_log,
                                     GOutputStream  *out,
                                     GError        **error);
gboolean Laminax_perf_log_dump_log    (LaminaxPerfLog   *perf_log,
                                     GOutputStream  *out,
                                     GError        **error);

G_END_DECLS

#endif /* __Laminax_PERF_LOG_H__ */
