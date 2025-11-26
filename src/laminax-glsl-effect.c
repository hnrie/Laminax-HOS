/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/**
 * SECTION:Laminax-glsl-effect
 * @short_description: An offscreen effect using GLSL
 *
 * A #LaminaxGLSLEffect is #ClutterOffscreenEffect that allows
 * running custom GLSL to the vertex fragment stages of the
 * graphic pipeline.
 */

#include "config.h"

#include <cogl/cogl.h>
#include "Laminax-glsl-effect.h"

typedef struct _LaminaxGLSLEffectPrivate LaminaxGLSLEffectPrivate;
struct _LaminaxGLSLEffectPrivate
{
  CoglPipeline  *pipeline;

  gint tex_width;
  gint tex_height;
};

G_DEFINE_TYPE_WITH_PRIVATE (LaminaxGLSLEffect, Laminax_glsl_effect, CLUTTER_TYPE_OFFSCREEN_EFFECT);

static gboolean
Laminax_glsl_effect_pre_paint (ClutterEffect      *effect,
                                ClutterPaintContext *paint_context)
{
  LaminaxGLSLEffect *self = Laminax_GLSL_EFFECT (effect);
  ClutterOffscreenEffect *offscreen_effect = CLUTTER_OFFSCREEN_EFFECT (effect);
  LaminaxGLSLEffectPrivate *priv = Laminax_glsl_effect_get_instance_private (self);
  ClutterEffectClass *parent_class;
  CoglHandle texture;
  gboolean success;

  if (!clutter_actor_meta_get_enabled (CLUTTER_ACTOR_META (effect)))
    return FALSE;

  if (!clutter_feature_available (CLUTTER_FEATURE_SHADERS_GLSL))
    {
      /* if we don't have support for GLSL shaders then we
       * forcibly disable the ActorMeta
       */
      g_warning ("Unable to use the ShaderEffect: the graphics hardware "
                 "or the current GL driver does not implement support "
                 "for the GLSL shading language.");
      clutter_actor_meta_set_enabled (CLUTTER_ACTOR_META (effect), FALSE);
      return FALSE;
    }

  parent_class = CLUTTER_EFFECT_CLASS (Laminax_glsl_effect_parent_class);
  success = parent_class->pre_paint (effect, paint_context);

  if (!success)
    return FALSE;

  texture = clutter_offscreen_effect_get_texture (offscreen_effect);
  priv->tex_width = cogl_texture_get_width (texture);
  priv->tex_height = cogl_texture_get_height (texture);

  cogl_pipeline_set_layer_texture (priv->pipeline, 0, texture);

  return TRUE;
}

static void
Laminax_glsl_effect_paint_target (ClutterOffscreenEffect *effect,
                                   ClutterPaintContext    *paint_context)
{
  LaminaxGLSLEffect *self = Laminax_GLSL_EFFECT (effect);
  LaminaxGLSLEffectPrivate *priv;
  ClutterActor *actor;
  guint8 paint_opacity;
  CoglFramebuffer *framebuffer;

  priv = Laminax_glsl_effect_get_instance_private (self);

  actor = clutter_actor_meta_get_actor (CLUTTER_ACTOR_META (effect));
  paint_opacity = clutter_actor_get_paint_opacity (actor);

  cogl_pipeline_set_color4ub (priv->pipeline,
                              paint_opacity,
                              paint_opacity,
                              paint_opacity,
                              paint_opacity);

  framebuffer = clutter_paint_context_get_framebuffer (paint_context);
  cogl_framebuffer_draw_rectangle (framebuffer,
                                   priv->pipeline,
                                   0, 0,
                                   priv->tex_width, priv->tex_height);
}


/**
 * Laminax_glsl_effect_add_glsl_snippet:
 * @effect: a #LaminaxGLSLEffect
 * @hook: where to insert the code
 * @declarations: GLSL declarations
 * @code: GLSL code
 * @is_replace: whether Cogl code should be replaced by the custom shader
 *
 * Adds a GLSL snippet to the pipeline used for drawing the effect texture.
 * See #CoglSnippet for details.
 *
 * This is only valid inside the a call to the build_pipeline() virtual
 * function.
 */
void
Laminax_glsl_effect_add_glsl_snippet (LaminaxGLSLEffect  *effect,
                                       LaminaxSnippetHook  hook,
                                       const char          *declarations,
                                       const char          *code,
                                       gboolean             is_replace)
{
  LaminaxGLSLEffectClass *klass = Laminax_GLSL_EFFECT_GET_CLASS (effect);
  CoglSnippet *snippet;

  g_return_if_fail (klass->base_pipeline != NULL);

  if (is_replace)
    {
      snippet = cogl_snippet_new (hook, declarations, NULL);
      cogl_snippet_set_replace (snippet, code);
    }
  else
    {
      snippet = cogl_snippet_new (hook, declarations, code);
    }

  if (hook == Laminax_SNIPPET_HOOK_VERTEX ||
      hook == Laminax_SNIPPET_HOOK_FRAGMENT)
    cogl_pipeline_add_snippet (klass->base_pipeline, snippet);
  else
    cogl_pipeline_add_layer_snippet (klass->base_pipeline, 0, snippet);

  cogl_object_unref (snippet);
}

static void
Laminax_glsl_effect_dispose (GObject *gobject)
{
  LaminaxGLSLEffect *self = Laminax_GLSL_EFFECT (gobject);
  LaminaxGLSLEffectPrivate *priv;

  priv = Laminax_glsl_effect_get_instance_private (self);

  g_clear_pointer (&priv->pipeline, cogl_object_unref);

  G_OBJECT_CLASS (Laminax_glsl_effect_parent_class)->dispose (gobject);
}

static void
Laminax_glsl_effect_init (LaminaxGLSLEffect *effect)
{
}

static void
Laminax_glsl_effect_constructed (GObject *object)
{
  LaminaxGLSLEffect *self;
  LaminaxGLSLEffectClass *klass;
  LaminaxGLSLEffectPrivate *priv;
  CoglContext *ctx =
    clutter_backend_get_cogl_context (clutter_get_default_backend ());

  G_OBJECT_CLASS (Laminax_glsl_effect_parent_class)->constructed (object);

  /* Note that, differently from ClutterBlurEffect, we are calling
     this inside constructed, not init, so klass points to the most-derived
     GTypeClass, not LaminaxGLSLEffectClass.
  */
  klass = Laminax_GLSL_EFFECT_GET_CLASS (object);
  self = Laminax_GLSL_EFFECT (object);
  priv = Laminax_glsl_effect_get_instance_private (self);

  if (G_UNLIKELY (klass->base_pipeline == NULL))
    {
      klass->base_pipeline = cogl_pipeline_new (ctx);
      cogl_pipeline_set_blend (klass->base_pipeline, "RGBA = ADD (SRC_COLOR * (SRC_COLOR[A]), DST_COLOR * (1-SRC_COLOR[A]))", NULL);

      if (klass->build_pipeline != NULL)
        klass->build_pipeline (self);
    }

  priv->pipeline = cogl_pipeline_copy (klass->base_pipeline);

  cogl_pipeline_set_layer_null_texture (klass->base_pipeline, 0);
}

static void
Laminax_glsl_effect_class_init (LaminaxGLSLEffectClass *klass)
{
  ClutterEffectClass *effect_class = CLUTTER_EFFECT_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterOffscreenEffectClass *offscreen_class;

  offscreen_class = CLUTTER_OFFSCREEN_EFFECT_CLASS (klass);
  offscreen_class->paint_target = Laminax_glsl_effect_paint_target;

  effect_class->pre_paint = Laminax_glsl_effect_pre_paint;

  gobject_class->constructed = Laminax_glsl_effect_constructed;
  gobject_class->dispose = Laminax_glsl_effect_dispose;
}

/**
 * Laminax_glsl_effect_get_uniform_location:
 * @effect: a #LaminaxGLSLEffect
 * @name: the uniform name
 *
 * Returns: the location of the uniform named @name, that can be
 *          passed to Laminax_glsl_effect_set_uniform_float().
 */
int
Laminax_glsl_effect_get_uniform_location (LaminaxGLSLEffect *effect,
                                           const char         *name)
{
  LaminaxGLSLEffectPrivate *priv = Laminax_glsl_effect_get_instance_private (effect);
  return cogl_pipeline_get_uniform_location (priv->pipeline, name);
}

/**
 * Laminax_glsl_effect_set_uniform_float:
 * @effect: a #LaminaxGLSLEffect
 * @uniform: the uniform location (as returned by Laminax_glsl_effect_get_uniform_location())
 * @n_components: the number of components in the uniform (eg. 3 for a vec3)
 * @total_count: the total number of floats in @value
 * @value: (array length=total_count): the array of floats to set @uniform
 */
void
Laminax_glsl_effect_set_uniform_float (LaminaxGLSLEffect *effect,
                                        int                 uniform,
                                        int                 n_components,
                                        int                 total_count,
                                        const float        *value)
{
  LaminaxGLSLEffectPrivate *priv = Laminax_glsl_effect_get_instance_private (effect);
  cogl_pipeline_set_uniform_float (priv->pipeline, uniform,
                                   n_components, total_count / n_components,
                                   value);
}
