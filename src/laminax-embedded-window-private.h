/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_EMBEDDED_WINDOW_PRIVATE_H__
#define __Laminax_EMBEDDED_WINDOW_PRIVATE_H__

#include "Laminax-embedded-window.h"
#include "Laminax-gtk-embed.h"

void _Laminax_embedded_window_set_actor (LaminaxEmbeddedWindow      *window,
                       LaminaxGtkEmbed            *embed);

void _Laminax_embedded_window_allocate (LaminaxEmbeddedWindow *window,
                      int                  x,
                      int                  y,
                      int                  width,
                      int                  height);

void _Laminax_embedded_window_map (LaminaxEmbeddedWindow *window);
void _Laminax_embedded_window_unmap (LaminaxEmbeddedWindow *window);

#endif /* __Laminax_EMBEDDED_WINDOW_PRIVATE_H__ */
