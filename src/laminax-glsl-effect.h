/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_GLSL_EFFECT_H__
#define __Laminax_GLSL_EFFECT_H__

#include "st.h"
#include <gtk/gtk.h>

/**
 * LaminaxSnippetHook:
 * Temporary hack to work around Cogl not exporting CoglSnippetHook in
 * the 1.0 API. Don't use.
 */
typedef enum {
  /* Per pipeline vertex hooks */
  Laminax_SNIPPET_HOOK_VERTEX = 0,
  Laminax_SNIPPET_HOOK_VERTEX_TRANSFORM,

  /* Per pipeline fragment hooks */
  Laminax_SNIPPET_HOOK_FRAGMENT = 2048,

  /* Per layer vertex hooks */
  Laminax_SNIPPET_HOOK_TEXTURE_COORD_TRANSFORM = 4096,

  /* Per layer fragment hooks */
  Laminax_SNIPPET_HOOK_LAYER_FRAGMENT = 6144,
  Laminax_SNIPPET_HOOK_TEXTURE_LOOKUP
} LaminaxSnippetHook;

#define Laminax_TYPE_GLSL_EFFECT (Laminax_glsl_effect_get_type ())
G_DECLARE_DERIVABLE_TYPE (LaminaxGLSLEffect, Laminax_glsl_effect, Laminax, GLSL_EFFECT, ClutterOffscreenEffect)

struct _LaminaxGLSLEffectClass
{
  ClutterOffscreenEffectClass parent_class;

  CoglPipeline *base_pipeline;

  void (*build_pipeline) (LaminaxGLSLEffect *effect);
};

void Laminax_glsl_effect_add_glsl_snippet (LaminaxGLSLEffect  *effect,
                                            LaminaxSnippetHook  hook,
                                            const char          *declarations,
                                            const char          *code,
                                            gboolean             is_replace);

int  Laminax_glsl_effect_get_uniform_location (LaminaxGLSLEffect *effect,
                                                const char         *name);
void Laminax_glsl_effect_set_uniform_float    (LaminaxGLSLEffect *effect,
                                                int                 uniform,
                                                int                 n_components,
                                                int                 total_count,
                                                const float        *value);

#endif /* __Laminax_GLSL_EFFECT_H__ */
