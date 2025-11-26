/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_GLOBAL_PRIVATE_H__
#define __Laminax_GLOBAL_PRIVATE_H__

#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "Laminax-global.h"
#include <gio/gio.h>
#include <girepository.h>
#include <meta/meta-plugin.h>


#include "Laminax-enum-types.h"
#include "Laminax-global-private.h"
#include "Laminax-perf-log.h"
#include "Laminax-window-tracker.h"
#include "Laminax-wm.h"
#include "st.h"

#include <cjs/gjs.h>

struct _LaminaxGlobal {
  GObject parent;

  ClutterStage *stage;

  MetaDisplay *meta_display;
  MetaWorkspaceManager *workspace_manager;
  Display *xdisplay;
  LaminaxScreen *Laminax_screen;

  LaminaxStageInputMode input_mode;
  XserverRegion input_region;

  GjsContext *js_context;
  MetaPlugin *plugin;
  LaminaxWM *wm;
  GSettings *settings;
  GSettings *interface_settings;
  const char *datadir;
  const char *imagedir;
  const char *userdatadir;
  StFocusManager *focus_manager;

  guint work_count;
  GSList *leisure_closures;
  guint leisure_function_id;

  gint64 last_gc_end_time;
  guint ui_scale;
  gboolean session_running;
  gboolean has_modal;

  guint notif_service_id;
};

void _Laminax_global_init            (const char *first_property_name,
                                    ...);
void _Laminax_global_set_plugin      (LaminaxGlobal  *global,
                                    MetaPlugin   *plugin);

GjsContext *_Laminax_global_get_gjs_context (LaminaxGlobal  *global);

#endif /* __Laminax_GLOBAL_PRIVATE_H__ */
