/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_STACK_H__
#define __Laminax_STACK_H__

#include "st.h"
#include <gtk/gtk.h>

#define Laminax_TYPE_STACK                 (Laminax_stack_get_type ())
#define Laminax_STACK(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), Laminax_TYPE_STACK, LaminaxStack))
#define Laminax_STACK_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_STACK, LaminaxStackClass))
#define Laminax_IS_STACK(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), Laminax_TYPE_STACK))
#define Laminax_IS_STACK_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_STACK))
#define Laminax_STACK_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_STACK, LaminaxStackClass))

typedef struct _LaminaxStack        LaminaxStack;
typedef struct _LaminaxStackClass   LaminaxStackClass;

typedef struct _LaminaxStackPrivate LaminaxStackPrivate;

struct _LaminaxStack
{
    StWidget parent;

    LaminaxStackPrivate *priv;
};

struct _LaminaxStackClass
{
    StWidgetClass parent_class;
};

GType Laminax_stack_get_type (void) G_GNUC_CONST;

#endif /* __Laminax_STACK_H__ */
