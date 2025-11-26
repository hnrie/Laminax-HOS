/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_SLICER_H__
#define __Laminax_SLICER_H__

#include "st.h"

#define Laminax_TYPE_SLICER                 (Laminax_slicer_get_type ())
#define Laminax_SLICER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), Laminax_TYPE_SLICER, LaminaxSlicer))
#define Laminax_SLICER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_SLICER, LaminaxSlicerClass))
#define Laminax_IS_SLICER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), Laminax_TYPE_SLICER))
#define Laminax_IS_SLICER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_SLICER))
#define Laminax_SLICER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_SLICER, LaminaxSlicerClass))

typedef struct _LaminaxSlicer        LaminaxSlicer;
typedef struct _LaminaxSlicerClass   LaminaxSlicerClass;

typedef struct _LaminaxSlicerPrivate LaminaxSlicerPrivate;

struct _LaminaxSlicer
{
    StBin parent;

    LaminaxSlicerPrivate *priv;
};

struct _LaminaxSlicerClass
{
    StBinClass parent_class;
};

GType Laminax_slicer_get_type (void) G_GNUC_CONST;

#endif /* __Laminax_SLICER_H__ */
