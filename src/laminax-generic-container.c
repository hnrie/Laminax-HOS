/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/**
 * SECTION:Laminax-generic-container
 * @short_description: A container class with signals for allocation
 *
 * #LaminaxGenericContainer is mainly a workaround for the current
 * lack of GObject subclassing + vfunc overrides in gjs.  We
 * implement the container interface, but proxy the virtual functions
 * into signals, which gjs can catch.
 *
 * #LaminaxGenericContainer is an #StWidget, and automatically takes its
 * borders and padding into account during size request and allocation.
 */

#include "config.h"

#include "Laminax-generic-container.h"

#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <girepository.h>

static void Laminax_generic_container_iface_init (ClutterContainerIface *iface);

G_DEFINE_TYPE_WITH_CODE(LaminaxGenericContainer,
                        Laminax_generic_container,
                        ST_TYPE_WIDGET,
                        G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTAINER,
                                               Laminax_generic_container_iface_init));

struct _LaminaxGenericContainerPrivate {
  GHashTable *skip_paint;
};

/* Signals */
enum
{
  GET_PREFERRED_WIDTH,
  GET_PREFERRED_HEIGHT,
  ALLOCATE,
  LAST_SIGNAL
};

static guint Laminax_generic_container_signals [LAST_SIGNAL] = { 0 };

static gpointer
Laminax_generic_container_allocation_ref (LaminaxGenericContainerAllocation *alloc)
{
  alloc->_refcount++;
  return alloc;
}

static void
Laminax_generic_container_allocation_unref (LaminaxGenericContainerAllocation *alloc)
{
  if (--alloc->_refcount == 0)
    g_slice_free (LaminaxGenericContainerAllocation, alloc);
}

static void
Laminax_generic_container_allocate (ClutterActor           *self,
                                  const ClutterActorBox  *box,
                                  ClutterAllocationFlags  flags)
{
  StThemeNode *theme_node;
  ClutterActorBox content_box;

  clutter_actor_set_allocation (self, box, flags);

  theme_node = st_widget_get_theme_node (ST_WIDGET (self));
  st_theme_node_get_content_box (theme_node, box, &content_box);

  g_signal_emit (G_OBJECT (self), Laminax_generic_container_signals[ALLOCATE], 0,
                 &content_box, flags);
}

static void
Laminax_generic_container_get_preferred_width (ClutterActor *actor,
                                             gfloat        for_height,
                                             gfloat       *min_width_p,
                                             gfloat       *natural_width_p)
{
  LaminaxGenericContainerAllocation *alloc = g_slice_new0 (LaminaxGenericContainerAllocation);
  StThemeNode *theme_node = st_widget_get_theme_node (ST_WIDGET (actor));

  st_theme_node_adjust_for_height (theme_node, &for_height);

  alloc->_refcount = 1;
  g_signal_emit (G_OBJECT (actor), Laminax_generic_container_signals[GET_PREFERRED_WIDTH], 0,
                 for_height, alloc);
  if (min_width_p)
    *min_width_p = alloc->min_size;
  if (natural_width_p)
    *natural_width_p = alloc->natural_size;
  Laminax_generic_container_allocation_unref (alloc);

  st_theme_node_adjust_preferred_width (theme_node, min_width_p, natural_width_p);
}

static void
Laminax_generic_container_get_preferred_height (ClutterActor *actor,
                                              gfloat        for_width,
                                              gfloat       *min_height_p,
                                              gfloat       *natural_height_p)
{
  LaminaxGenericContainerAllocation *alloc = g_slice_new0 (LaminaxGenericContainerAllocation);
  StThemeNode *theme_node = st_widget_get_theme_node (ST_WIDGET (actor));

  st_theme_node_adjust_for_width (theme_node, &for_width);

  alloc->_refcount = 1;
  g_signal_emit (G_OBJECT (actor), Laminax_generic_container_signals[GET_PREFERRED_HEIGHT], 0,
                 for_width, alloc);
  if (min_height_p)
    *min_height_p = alloc->min_size;
  if (natural_height_p)
    *natural_height_p = alloc->natural_size;
  Laminax_generic_container_allocation_unref (alloc);

  st_theme_node_adjust_preferred_height (theme_node, min_height_p, natural_height_p);
}

static void
Laminax_generic_container_paint (ClutterActor  *actor,
                                  ClutterPaintContext *paint_context)
{
  LaminaxGenericContainer *self = (LaminaxGenericContainer*) actor;
  ClutterActor *child;

  st_widget_paint_background (ST_WIDGET (actor), paint_context);

  for (child = clutter_actor_get_first_child (actor);
       child != NULL;
       child = clutter_actor_get_next_sibling (child))
    {
      if (g_hash_table_lookup (self->priv->skip_paint, child))
        continue;

      clutter_actor_paint (child, paint_context);
    }
}

static void
Laminax_generic_container_pick (ClutterActor        *actor,
                                 ClutterPickContext  *pick_context)
{
  LaminaxGenericContainer *self = (LaminaxGenericContainer*) actor;
  ClutterActor *child;

  CLUTTER_ACTOR_CLASS (Laminax_generic_container_parent_class)->pick (actor, pick_context);

  for (child = clutter_actor_get_first_child (actor);
       child != NULL;
       child = clutter_actor_get_next_sibling (child))
    {
      if (g_hash_table_lookup (self->priv->skip_paint, child))
        continue;

      clutter_actor_pick (child, pick_context);
    }
}

static GList *
Laminax_generic_container_get_focus_chain (StWidget *widget)
{
  LaminaxGenericContainer *self = Laminax_GENERIC_CONTAINER (widget);
  ClutterActor *child;
  GList *focus_chain;

  focus_chain = NULL;
  for (child = clutter_actor_get_first_child (CLUTTER_ACTOR (self));
       child != NULL;
       child = clutter_actor_get_next_sibling (child))
    {
      if (clutter_actor_is_visible (child) &&
          !Laminax_generic_container_get_skip_paint (self, child))
        focus_chain = g_list_prepend (focus_chain, child);
    }

  return g_list_reverse (focus_chain);
}

/**
 * Laminax_generic_container_get_n_skip_paint:
 * @self:  A #LaminaxGenericContainer
 *
 * Returns: Number of children which will not be painted.
 */
guint
Laminax_generic_container_get_n_skip_paint (LaminaxGenericContainer  *self)
{
  return g_hash_table_size (self->priv->skip_paint);
}

/**
 * Laminax_generic_container_get_skip_paint:
 * @self: A #LaminaxGenericContainer
 * @child: Child #ClutterActor
 *
 * Gets whether or not @actor is skipped when painting.
 *
 * Return value: %TRUE or %FALSE
 */
gboolean
Laminax_generic_container_get_skip_paint (LaminaxGenericContainer  *self,
                                        ClutterActor           *child)
{
  return g_hash_table_lookup (self->priv->skip_paint, child) != NULL;
}

/**
 * Laminax_generic_container_set_skip_paint:
 * @self: A #LaminaxGenericContainer
 * @child: Child #ClutterActor
 * @skip: %TRUE if we should skip painting
 *
 * Set whether or not we should skip painting @actor.  Workaround for
 * lack of gjs ability to override _paint vfunc.
 */
void
Laminax_generic_container_set_skip_paint (LaminaxGenericContainer  *self,
                                        ClutterActor           *child,
                                        gboolean                skip)
{
  gboolean currently_skipping;

  currently_skipping = g_hash_table_lookup (self->priv->skip_paint, child) != NULL;
  if (!!skip == currently_skipping)
    return;

  if (!skip)
    g_hash_table_remove (self->priv->skip_paint, child);
  else
    g_hash_table_insert (self->priv->skip_paint, child, child);

  clutter_actor_queue_redraw (CLUTTER_ACTOR (self));
}

static gboolean
Laminax_generic_container_get_paint_volume (ClutterActor *self,
                                          ClutterPaintVolume *volume)
{
  ClutterActorBox paint_box, alloc_box;
  StThemeNode *theme_node;
  graphene_point3d_t origin;

  /* Setting the paint volume does not make sense when we don't have any allocation */
  if (!clutter_actor_has_allocation (self))
    return FALSE;

  theme_node = st_widget_get_theme_node (ST_WIDGET (self));
  clutter_actor_get_allocation_box (self, &alloc_box);

  st_theme_node_get_paint_box (theme_node, &alloc_box, &paint_box);

  origin.x = paint_box.x1 - alloc_box.x1;
  origin.y = paint_box.y1 - alloc_box.y1;
  origin.z = 0.0f;

  clutter_paint_volume_set_origin (volume, &origin);
  clutter_paint_volume_set_width (volume, paint_box.x2 - paint_box.x1);
  clutter_paint_volume_set_height (volume, paint_box.y2 - paint_box.y1);

  if (!clutter_actor_get_clip_to_allocation (self))
    {
      ClutterActor *child;
      /* Based on ClutterGroup/ClutterBox; include the children's
       * paint volumes, since they may paint outside our allocation.
       */
      for (child = clutter_actor_get_first_child (self);
           child != NULL;
           child = clutter_actor_get_next_sibling (child))
        {
          const ClutterPaintVolume *child_volume;

          if (!clutter_actor_is_visible (child))
            continue;

          if (Laminax_generic_container_get_skip_paint (Laminax_GENERIC_CONTAINER  (self), child))
            continue;

          child_volume = clutter_actor_get_transformed_paint_volume (child, self);
          if (!child_volume)
            return FALSE;

          clutter_paint_volume_union (volume, child_volume);
        }
    }

  return TRUE;
}

static void
Laminax_generic_container_finalize (GObject *object)
{
  LaminaxGenericContainer *self = (LaminaxGenericContainer*) object;

  g_hash_table_destroy (self->priv->skip_paint);

  G_OBJECT_CLASS (Laminax_generic_container_parent_class)->finalize (object);
}

static void
Laminax_generic_container_class_init (LaminaxGenericContainerClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  StWidgetClass *widget_class = ST_WIDGET_CLASS (klass);

  gobject_class->finalize = Laminax_generic_container_finalize;

  actor_class->get_preferred_width = Laminax_generic_container_get_preferred_width;
  actor_class->get_preferred_height = Laminax_generic_container_get_preferred_height;
  actor_class->allocate = Laminax_generic_container_allocate;
  actor_class->paint = Laminax_generic_container_paint;
  actor_class->pick = Laminax_generic_container_pick;
  actor_class->get_paint_volume = Laminax_generic_container_get_paint_volume;

  widget_class->get_focus_chain = Laminax_generic_container_get_focus_chain;

  /**
   * LaminaxGenericContainer::get-preferred-width:
   * @self: the #LaminaxGenericContainer
   * @for_height: as in clutter_actor_get_preferred_width()
   * @alloc: a #LaminaxGenericContainerAllocation to be filled in
   *
   * Emitted when clutter_actor_get_preferred_width() is called
   * on @self. You should fill in the fields of @alloc with the
   * your minimum and natural widths. #LaminaxGenericContainer
   * will deal with taking its borders and padding into account
   * for you.
   *
   * @alloc's fields are initialized to 0, so unless you have a fixed
   * width specified (via #ClutterActor:width or CSS), you must
   * connect to this signal and fill in the values.
   */
  Laminax_generic_container_signals[GET_PREFERRED_WIDTH] =
    g_signal_new ("get-preferred-width",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 2, G_TYPE_FLOAT, Laminax_TYPE_GENERIC_CONTAINER_ALLOCATION);

  /**
   * LaminaxGenericContainer::get-preferred-height:
   * @self: the #LaminaxGenericContainer
   * @for_width: as in clutter_actor_get_preferred_height()
   * @alloc: a #LaminaxGenericContainerAllocation to be filled in
   *
   * Emitted when clutter_actor_get_preferred_height() is called
   * on @self. You should fill in the fields of @alloc with the
   * your minimum and natural heights. #LaminaxGenericContainer
   * will deal with taking its borders and padding into account
   * for you.
   *
   * @alloc's fields are initialized to 0, so unless you have a fixed
   * height specified (via #ClutterActor:height or CSS), you must
   * connect to this signal and fill in the values.
   */
  Laminax_generic_container_signals[GET_PREFERRED_HEIGHT] =
    g_signal_new ("get-preferred-height",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 2, G_TYPE_FLOAT, Laminax_TYPE_GENERIC_CONTAINER_ALLOCATION);

  /**
   * LaminaxGenericContainer::allocate:
   * @self: the #LaminaxGenericContainer
   * @box: @self's content box
   * @flags: the allocation flags.
   *
   * Emitted when @self is allocated, after chaining up to the parent
   * allocate method.
   *
   * Note that @box is @self's content box (qv
   * st_theme_node_get_content_box()), NOT its allocation.
   */
  Laminax_generic_container_signals[ALLOCATE] =
    g_signal_new ("allocate",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 2, CLUTTER_TYPE_ACTOR_BOX, CLUTTER_TYPE_ALLOCATION_FLAGS);

  g_type_class_add_private (gobject_class, sizeof (LaminaxGenericContainerPrivate));
}

static void
Laminax_generic_container_actor_removed (ClutterContainer *container,
                                       ClutterActor     *actor)
{
  LaminaxGenericContainerPrivate *priv = Laminax_GENERIC_CONTAINER (container)->priv;

  g_hash_table_remove (priv->skip_paint, actor);
}

static void
Laminax_generic_container_iface_init (ClutterContainerIface *iface)
{
  iface->actor_removed = Laminax_generic_container_actor_removed;
}

static void
Laminax_generic_container_init (LaminaxGenericContainer *area)
{
  area->priv = G_TYPE_INSTANCE_GET_PRIVATE (area, Laminax_TYPE_GENERIC_CONTAINER,
                                            LaminaxGenericContainerPrivate);
  area->priv->skip_paint = g_hash_table_new (NULL, NULL);
}

GType
Laminax_generic_container_allocation_get_type (void)
{
  static GType gtype = G_TYPE_INVALID;
  if (gtype == G_TYPE_INVALID)
    {
      gtype = g_boxed_type_register_static ("LaminaxGenericContainerAllocation",
         (GBoxedCopyFunc)Laminax_generic_container_allocation_ref,
         (GBoxedFreeFunc)Laminax_generic_container_allocation_unref);
    }
  return gtype;
}
