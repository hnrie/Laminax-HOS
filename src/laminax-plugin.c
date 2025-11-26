/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (c) 2008 Red Hat, Inc.
 * Copyright (c) 2008 Intel Corp.
 *
 * Based on plugin skeleton by:
 * Author: Tomas Frydrych <tf@linux.intel.com>
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * LaminaxPlugin is the entry point for for Laminax into and out of
 * Mutter. By registering itself into Mutter using
 * meta_plugin_manager_set_plugin_type(), Mutter will call the vfuncs of the
 * plugin at the appropriate time.
 *
 * The functions in in LaminaxPlugin are all just stubs, which just call
 * the similar methods in LaminaxWm.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <cjs/gjs.h>
#include <meta/display.h>
#include <meta/meta-plugin.h>
#include <meta/util.h>

#include "Laminax-global-private.h"
#include "Laminax-perf-log.h"
#include "Laminax-wm-private.h"

#define Laminax_TYPE_PLUGIN (Laminax_plugin_get_type ())
G_DECLARE_FINAL_TYPE (LaminaxPlugin, Laminax_plugin,
                      Laminax, PLUGIN,
                      MetaPlugin)

struct _LaminaxPlugin
{
  MetaPlugin parent;

  int glx_error_base;
  int glx_event_base;
  guint have_swap_event : 1;
  CoglContext *cogl_context;

  LaminaxGlobal *global;
};

G_DEFINE_TYPE (LaminaxPlugin, Laminax_plugin, META_TYPE_PLUGIN)

static gboolean
Laminax_plugin_has_swap_event (LaminaxPlugin *Laminax_plugin)
{
  if (meta_is_wayland_compositor ())
  {
    return FALSE;
  }

  CoglDisplay *cogl_display =
    cogl_context_get_display (Laminax_plugin->cogl_context);
  CoglRenderer *renderer = cogl_display_get_renderer (cogl_display);
  const char * (* query_extensions_string) (Display *dpy, int screen);
  Bool (* query_extension) (Display *dpy, int *error, int *event);
  Display *xdisplay;
  int screen_number;
  const char *glx_extensions;

  /* We will only get swap events if Cogl is using GLX */
  if (cogl_renderer_get_winsys_id (renderer) != COGL_WINSYS_ID_GLX)
    return FALSE;

  xdisplay = clutter_x11_get_default_display ();

  query_extensions_string =
    (void *) cogl_get_proc_address ("glXQueryExtensionsString");
  query_extension =
    (void *) cogl_get_proc_address ("glXQueryExtension");

  query_extension (xdisplay,
                   &Laminax_plugin->glx_error_base,
                   &Laminax_plugin->glx_event_base);

  screen_number = XDefaultScreen (xdisplay);
  glx_extensions = query_extensions_string (xdisplay, screen_number);

  return strstr (glx_extensions, "GLX_INTEL_swap_event") != NULL;
}

static void
Laminax_plugin_start (MetaPlugin *plugin)
{
  LaminaxPlugin *Laminax_plugin = Laminax_PLUGIN (plugin);
  GError *error = NULL;
  int status;
  GjsContext *gjs_context;
  ClutterBackend *backend;

  backend = clutter_get_default_backend ();
  Laminax_plugin->cogl_context = clutter_backend_get_cogl_context (backend);

  Laminax_plugin->have_swap_event =
    Laminax_plugin_has_swap_event (Laminax_plugin);

  Laminax_perf_log_define_event (Laminax_perf_log_get_default (),
                               "glx.swapComplete",
                               "GL buffer swap complete event received (with timestamp of completion)",
                               "x");

  Laminax_plugin->global = Laminax_global_get ();
  _Laminax_global_set_plugin (Laminax_plugin->global, META_PLUGIN (Laminax_plugin));

  gjs_context = _Laminax_global_get_gjs_context (Laminax_plugin->global);

  if (!gjs_context_eval (gjs_context,
                         "imports.ui.environment.init();"
                         "imports.ui.main.start();",
                         -1,
                         "<main>",
                         &status,
                         &error))
    {
      g_message ("Execution of main.js threw exception: %s", error->message);
      g_error_free (error);
      /* We just exit() here, since in a development environment you'll get the
       * error in your shell output, and it's way better than a busted WM,
       * which typically manifests as a white screen.
       *
       * In production, we shouldn't crash =)  But if we do, we should get
       * restarted by the session infrastructure, which is likely going
       * to be better than some undefined state.
       *
       * If there was a generic "hook into bug-buddy for non-C crashes"
       * infrastructure, here would be the place to put it.
       */
      g_object_unref (gjs_context);
      exit (1);
    }
}

static LaminaxWM *
get_Laminax_wm (void)
{
  LaminaxWM *wm;

  g_object_get (Laminax_global_get (),
                "window-manager", &wm,
                NULL);
  /* drop extra ref added by g_object_get */
  g_object_unref (wm);

  return wm;
}

static void
Laminax_plugin_minimize (MetaPlugin         *plugin,
                 MetaWindowActor    *actor)
{
  _Laminax_wm_minimize (get_Laminax_wm (),
                      actor);

}

static void
Laminax_plugin_unminimize (MetaPlugin         *plugin,
                               MetaWindowActor    *actor)
{
  _Laminax_wm_unminimize (get_Laminax_wm (),
                      actor);

}

static void
Laminax_plugin_size_changed (MetaPlugin         *plugin,
                                 MetaWindowActor    *actor)
{
  _Laminax_wm_size_changed (get_Laminax_wm (), actor);
}

static void
Laminax_plugin_size_change (MetaPlugin         *plugin,
                                MetaWindowActor    *actor,
                                MetaSizeChange      which_change,
                                MetaRectangle      *old_frame_rect,
                                MetaRectangle      *old_buffer_rect)
{
  _Laminax_wm_size_change (get_Laminax_wm (), actor, which_change, old_frame_rect, old_buffer_rect);
}

static void
Laminax_plugin_map (MetaPlugin         *plugin,
                        MetaWindowActor    *actor)
{
  _Laminax_wm_map (get_Laminax_wm (),
                 actor);
}

static void
Laminax_plugin_destroy (MetaPlugin         *plugin,
                            MetaWindowActor    *actor)
{
  _Laminax_wm_destroy (get_Laminax_wm (),
                     actor);
}

static void
Laminax_plugin_switch_workspace (MetaPlugin         *plugin,
                                     gint                from,
                                     gint                to,
                                     MetaMotionDirection direction)
{
  _Laminax_wm_switch_workspace (get_Laminax_wm(), from, to, direction);
}

static void
Laminax_plugin_kill_window_effects (MetaPlugin         *plugin,
                                        MetaWindowActor    *actor)
{
  _Laminax_wm_kill_window_effects (get_Laminax_wm(), actor);
}

static void
Laminax_plugin_kill_switch_workspace (MetaPlugin         *plugin)
{
  _Laminax_wm_kill_switch_workspace (get_Laminax_wm());
}

static void
Laminax_plugin_show_tile_preview (MetaPlugin      *plugin,
                                      MetaWindow      *window,
                                      MetaRectangle   *tile_rect,
                                      int              tile_monitor)
{
  _Laminax_wm_show_tile_preview (get_Laminax_wm (), window, tile_rect, tile_monitor);
}

static void
Laminax_plugin_hide_tile_preview (MetaPlugin *plugin)
{
  _Laminax_wm_hide_tile_preview (get_Laminax_wm ());
}

static void
Laminax_plugin_show_window_menu (MetaPlugin         *plugin,
                                     MetaWindow         *window,
                                     MetaWindowMenuType  menu,
                                     int                 x,
                                     int                 y)
{
  _Laminax_wm_show_window_menu (get_Laminax_wm (), window, menu, x, y);
}

static void
Laminax_plugin_show_window_menu_for_rect (MetaPlugin         *plugin,
                                              MetaWindow         *window,
                                              MetaWindowMenuType  menu,
                                              MetaRectangle      *rect)
{
  _Laminax_wm_show_window_menu_for_rect (get_Laminax_wm (), window, menu, rect);
}

static gboolean
Laminax_plugin_xevent_filter (MetaPlugin *plugin,
                                  XEvent     *xev)
{
#ifdef GLX_INTEL_swap_event
  LaminaxPlugin *Laminax_plugin = Laminax_PLUGIN (plugin);

  if (Laminax_plugin->have_swap_event &&
      xev->type == (Laminax_plugin->glx_event_base + GLX_BufferSwapComplete))
    {
      GLXBufferSwapComplete *swap_complete_event;
      swap_complete_event = (GLXBufferSwapComplete *)xev;

      /* Buggy early versions of the INTEL_swap_event implementation in Mesa
       * can send this with a ust of 0. Simplify life for consumers
       * by ignoring such events */
      if (swap_complete_event->ust != 0)
        {
          gboolean frame_timestamps;
          g_object_get (Laminax_plugin->global,
                        "frame-timestamps", &frame_timestamps,
                        NULL);

          if (frame_timestamps)
            Laminax_perf_log_event_x (Laminax_perf_log_get_default (),
                                    "glx.swapComplete",
                                    swap_complete_event->ust);
        }
    }
#endif

  return FALSE;
}

static gboolean
Laminax_plugin_keybinding_filter (MetaPlugin     *plugin,
                                      MetaKeyBinding *binding)
{
  return _Laminax_wm_filter_keybinding (get_Laminax_wm (), binding);
}

static void
Laminax_plugin_confirm_display_change (MetaPlugin *plugin)
{
  _Laminax_wm_confirm_display_change (get_Laminax_wm ());
}

static const MetaPluginInfo *
Laminax_plugin_plugin_info (MetaPlugin *plugin)
{
  static const MetaPluginInfo info = {
    .name = "Laminax",
    .version = "0.1",
    .author = "Various",
    .license = "GPLv2+",
    .description = "Provides Laminax core functionality"
  };

  return &info;
}

static MetaCloseDialog *
Laminax_plugin_create_close_dialog (MetaPlugin *plugin,
                                        MetaWindow *window)
{
  return _Laminax_wm_create_close_dialog (get_Laminax_wm (), window);
}

static MetaInhibitShortcutsDialog *
Laminax_plugin_create_inhibit_shortcuts_dialog (MetaPlugin *plugin,
                                                    MetaWindow *window)
{
  return _Laminax_wm_create_inhibit_shortcuts_dialog (get_Laminax_wm (), window);
}

static void
Laminax_plugin_locate_pointer (MetaPlugin *plugin)
{
  LaminaxPlugin *Laminax_plugin = Laminax_PLUGIN (plugin);
  // TODO
  // _Laminax_global_locate_pointer (Laminax_plugin->global);
}

static void
Laminax_plugin_class_init (LaminaxPluginClass *klass)
{
  MetaPluginClass *plugin_class  = META_PLUGIN_CLASS (klass);

  plugin_class->start            = Laminax_plugin_start;
  plugin_class->map              = Laminax_plugin_map;
  plugin_class->minimize         = Laminax_plugin_minimize;
  plugin_class->unminimize       = Laminax_plugin_unminimize;
  plugin_class->size_changed     = Laminax_plugin_size_changed;
  plugin_class->size_change      = Laminax_plugin_size_change;
  plugin_class->destroy          = Laminax_plugin_destroy;

  plugin_class->switch_workspace = Laminax_plugin_switch_workspace;

  plugin_class->kill_window_effects   = Laminax_plugin_kill_window_effects;
  plugin_class->kill_switch_workspace = Laminax_plugin_kill_switch_workspace;

  plugin_class->show_tile_preview = Laminax_plugin_show_tile_preview;
  plugin_class->hide_tile_preview = Laminax_plugin_hide_tile_preview;
  plugin_class->show_window_menu = Laminax_plugin_show_window_menu;
  plugin_class->show_window_menu_for_rect = Laminax_plugin_show_window_menu_for_rect;

  plugin_class->xevent_filter     = Laminax_plugin_xevent_filter;
  plugin_class->keybinding_filter = Laminax_plugin_keybinding_filter;

  plugin_class->confirm_display_change = Laminax_plugin_confirm_display_change;

  plugin_class->plugin_info       = Laminax_plugin_plugin_info;

  plugin_class->create_close_dialog = Laminax_plugin_create_close_dialog;
  plugin_class->create_inhibit_shortcuts_dialog = Laminax_plugin_create_inhibit_shortcuts_dialog;

  plugin_class->locate_pointer = Laminax_plugin_locate_pointer;
}

static void
Laminax_plugin_init (LaminaxPlugin *Laminax_plugin)
{
}
