/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_GTK_EMBED_H__
#define __Laminax_GTK_EMBED_H__

#include <clutter/clutter.h>

#include "Laminax-embedded-window.h"

#define Laminax_TYPE_GTK_EMBED (Laminax_gtk_embed_get_type ())
G_DECLARE_DERIVABLE_TYPE (LaminaxGtkEmbed, Laminax_gtk_embed,
                          Laminax, GTK_EMBED, ClutterClone)

struct _LaminaxGtkEmbedClass
{
    ClutterCloneClass parent_class;
};

ClutterActor *Laminax_gtk_embed_new (LaminaxEmbeddedWindow *window);

#endif /* __Laminax_GTK_EMBED_H__ */
