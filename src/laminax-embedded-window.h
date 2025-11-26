/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_EMBEDDED_WINDOW_H__
#define __Laminax_EMBEDDED_WINDOW_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

#define Laminax_TYPE_EMBEDDED_WINDOW (Laminax_embedded_window_get_type ())
G_DECLARE_DERIVABLE_TYPE (LaminaxEmbeddedWindow, Laminax_embedded_window,
                          Laminax, EMBEDDED_WINDOW, GtkWindow)

struct _LaminaxEmbeddedWindowClass
{
  GtkWindowClass parent_class;
};

GtkWidget *Laminax_embedded_window_new (void);

#endif /* __Laminax_EMBEDDED_WINDOW_H__ */
