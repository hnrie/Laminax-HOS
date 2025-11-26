/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_DOC_SYSTEM_H__
#define __Laminax_DOC_SYSTEM_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

#define Laminax_TYPE_DOC_SYSTEM                 (Laminax_doc_system_get_type ())
#define Laminax_DOC_SYSTEM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), Laminax_TYPE_DOC_SYSTEM, LaminaxDocSystem))
#define Laminax_DOC_SYSTEM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_DOC_SYSTEM, LaminaxDocSystemClass))
#define Laminax_IS_DOC_SYSTEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), Laminax_TYPE_DOC_SYSTEM))
#define Laminax_IS_DOC_SYSTEM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_DOC_SYSTEM))
#define Laminax_DOC_SYSTEM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_DOC_SYSTEM, LaminaxDocSystemClass))

typedef struct _LaminaxDocSystem LaminaxDocSystem;
typedef struct _LaminaxDocSystemClass LaminaxDocSystemClass;
typedef struct _LaminaxDocSystemPrivate LaminaxDocSystemPrivate;

struct _LaminaxDocSystem
{
  GObject parent;

  LaminaxDocSystemPrivate *priv;
};

struct _LaminaxDocSystemClass
{
  GObjectClass parent_class;
};

GType Laminax_doc_system_get_type (void) G_GNUC_CONST;

LaminaxDocSystem* Laminax_doc_system_get_default (void);

GSList *Laminax_doc_system_get_all (LaminaxDocSystem    *system);

#endif /* __Laminax_DOC_SYSTEM_H__ */
