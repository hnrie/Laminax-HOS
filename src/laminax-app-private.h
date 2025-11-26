/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_APP_PRIVATE_H__
#define __Laminax_APP_PRIVATE_H__

#include "Laminax-app.h"
#include "Laminax-app-system.h"

G_BEGIN_DECLS

LaminaxApp* _Laminax_app_new_for_window (MetaWindow *window);

LaminaxApp* _Laminax_app_new (GMenuTreeEntry *entry);

void _Laminax_app_set_entry (LaminaxApp *app, GMenuTreeEntry *entry);

void _Laminax_app_handle_startup_sequence (LaminaxApp *app, MetaStartupSequence *sequence);

void _Laminax_app_add_window (LaminaxApp *app, MetaWindow *window);

void _Laminax_app_remove_window (LaminaxApp *app, MetaWindow *window);

void _Laminax_app_do_match (LaminaxApp         *app,
                          GSList           *terms,
                          GSList          **prefix_results,
                          GSList          **substring_results);
const char * _Laminax_app_get_common_name (LaminaxApp *app);
void         _Laminax_app_set_unique_name (LaminaxApp *app, gchar *unique_name);
const char * _Laminax_app_get_unique_name (LaminaxApp *app);
const char * _Laminax_app_get_executable (LaminaxApp *app);
const char * _Laminax_app_get_desktop_path (LaminaxApp *app);
void         _Laminax_app_set_hidden_as_duplicate (LaminaxApp *app, gboolean hide);
G_END_DECLS

#endif /* __Laminax_APP_PRIVATE_H__ */
