#ifndef __Laminax_TOUCHEGG_CLIENT_H__
#define __Laminax_TOUCHEGG_CLIENT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define Laminax_TYPE_TOUCHEGG_CLIENT (Laminax_touchegg_client_get_type ())
G_DECLARE_FINAL_TYPE (LaminaxToucheggClient, Laminax_touchegg_client, Laminax, TOUCHEGG_CLIENT, GObject)

LaminaxToucheggClient  *Laminax_touchegg_client_new (void);

G_END_DECLS

#endif  /* __Laminax_TOUCHEGG_CLIENT_H__ */
