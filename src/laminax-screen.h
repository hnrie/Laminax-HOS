/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2008 Iain Holmes
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Suite 500, Boston, MA
 * 02110-1335, USA.
 */

#ifndef Laminax_SCREEN_H
#define Laminax_SCREEN_H

#include <glib-object.h>
#include <meta/types.h>
#include <meta/workspace.h>
#include <meta/display.h>
#include <meta/meta-workspace-manager.h>

#define Laminax_TYPE_SCREEN (Laminax_screen_get_type ())
G_DECLARE_FINAL_TYPE (LaminaxScreen, Laminax_screen, Laminax, SCREEN, GObject)

LaminaxScreen *Laminax_screen_new (MetaDisplay *display);

MetaDisplay *Laminax_screen_get_display (LaminaxScreen *screen);

void Laminax_screen_get_size (LaminaxScreen *screen,
                           int        *width,
                           int        *height);

GList *Laminax_screen_get_workspaces (LaminaxScreen *screen);

int Laminax_screen_get_n_workspaces (LaminaxScreen *screen);

MetaWorkspace* Laminax_screen_get_workspace_by_index (LaminaxScreen    *screen,
                                                   int            index);
void Laminax_screen_remove_workspace (LaminaxScreen    *screen,
                                   MetaWorkspace *workspace,
                                   guint32        timestamp);

MetaWorkspace *Laminax_screen_append_new_workspace (LaminaxScreen    *screen,
                                                 gboolean       activate,
                                                 guint32        timestamp);

int Laminax_screen_get_active_workspace_index (LaminaxScreen *screen);

MetaWorkspace * Laminax_screen_get_active_workspace (LaminaxScreen *screen);

void Laminax_screen_show_desktop (LaminaxScreen *screen,
                                   guint32         timestamp);

void Laminax_screen_toggle_desktop (LaminaxScreen *screen,
                                     guint32         timestamp);
                                
void Laminax_screen_unshow_desktop (LaminaxScreen *screen);

int  Laminax_screen_get_n_monitors       (LaminaxScreen    *screen);
int  Laminax_screen_get_primary_monitor  (LaminaxScreen    *screen);
int  Laminax_screen_get_current_monitor  (LaminaxScreen    *screen);
void Laminax_screen_get_monitor_geometry (LaminaxScreen    *screen,
                                       int            monitor,
                                       MetaRectangle *geometry);

gboolean Laminax_screen_get_monitor_in_fullscreen (LaminaxScreen  *screen,
                                                int          monitor);

int Laminax_screen_get_monitor_index_for_rect (LaminaxScreen    *screen,
                                            MetaRectangle *rect);

MetaWindow* Laminax_screen_get_mouse_window (LaminaxScreen *screen,
                                              MetaWindow *not_this_one);

void Laminax_screen_override_workspace_layout (LaminaxScreen      *screen,
                                                MetaDisplayCorner starting_corner,
                                                gboolean         vertical_layout,
                                                int              n_rows,
                                                int              n_columns);

unsigned long Laminax_screen_get_xwindow_for_window (LaminaxScreen *screen,
                                                      MetaWindow     *window);

#endif
