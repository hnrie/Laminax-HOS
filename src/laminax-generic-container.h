/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_GENERIC_CONTAINER_H__
#define __Laminax_GENERIC_CONTAINER_H__

#include "st.h"

#define Laminax_TYPE_GENERIC_CONTAINER                 (Laminax_generic_container_get_type ())
#define Laminax_GENERIC_CONTAINER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), Laminax_TYPE_GENERIC_CONTAINER, LaminaxGenericContainer))
#define Laminax_GENERIC_CONTAINER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_GENERIC_CONTAINER, LaminaxGenericContainerClass))
#define Laminax_IS_GENERIC_CONTAINER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), Laminax_TYPE_GENERIC_CONTAINER))
#define Laminax_IS_GENERIC_CONTAINER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_GENERIC_CONTAINER))
#define Laminax_GENERIC_CONTAINER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_GENERIC_CONTAINER, LaminaxGenericContainerClass))

typedef struct {
  float min_size;
  float natural_size;

  /* <private> */
  guint _refcount;
} LaminaxGenericContainerAllocation;

#define Laminax_TYPE_GENERIC_CONTAINER_ALLOCATION (Laminax_generic_container_allocation_get_type ())
GType Laminax_generic_container_allocation_get_type (void);

typedef struct _LaminaxGenericContainer        LaminaxGenericContainer;
typedef struct _LaminaxGenericContainerClass   LaminaxGenericContainerClass;

typedef struct _LaminaxGenericContainerPrivate LaminaxGenericContainerPrivate;

struct _LaminaxGenericContainer
{
    StWidget parent;

    LaminaxGenericContainerPrivate *priv;
};

struct _LaminaxGenericContainerClass
{
    StWidgetClass parent_class;
};

GType    Laminax_generic_container_get_type         (void) G_GNUC_CONST;

guint    Laminax_generic_container_get_n_skip_paint (LaminaxGenericContainer *self);

gboolean Laminax_generic_container_get_skip_paint   (LaminaxGenericContainer *self,
                                                   ClutterActor          *child);
void     Laminax_generic_container_set_skip_paint   (LaminaxGenericContainer *self,
                                                   ClutterActor          *child,
                                                   gboolean               skip);

#endif /* __Laminax_GENERIC_CONTAINER_H__ */
