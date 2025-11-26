/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
#ifndef __Laminax_NETWORK_AGENT_H__
#define __Laminax_NETWORK_AGENT_H__

#include <glib-object.h>
#include <glib.h>
#include <NetworkManager.h>
#include <nm-secret-agent-old.h>

G_BEGIN_DECLS

typedef enum {
  Laminax_NETWORK_AGENT_CONFIRMED,
  Laminax_NETWORK_AGENT_USER_CANCELED,
  Laminax_NETWORK_AGENT_INTERNAL_ERROR
} LaminaxNetworkAgentResponse;

typedef struct _LaminaxNetworkAgent         LaminaxNetworkAgent;
typedef struct _LaminaxNetworkAgentClass    LaminaxNetworkAgentClass;
typedef struct _LaminaxNetworkAgentPrivate  LaminaxNetworkAgentPrivate;

#define Laminax_TYPE_NETWORK_AGENT                  (Laminax_network_agent_get_type ())
#define Laminax_NETWORK_AGENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), Laminax_TYPE_NETWORK_AGENT, LaminaxNetworkAgent))
#define Laminax_IS_NETWORK_AGENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), Laminax_TYPE_NETWORK_AGENT))
#define Laminax_NETWORK_AGENT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), Laminax_TYPE_NETWORK_AGENT, LaminaxNetworkAgentClass))
#define Laminax_IS_NETWORK_AGENT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), Laminax_TYPE_NETWORK_AGENT))
#define Laminax_NETWORK_AGENT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), Laminax_TYPE_NETWORK_AGENT, LaminaxNetworkAgentClass))

struct _LaminaxNetworkAgent
{
  /*< private >*/
  NMSecretAgentOld parent_instance;

  LaminaxNetworkAgentPrivate *priv;
};

struct _LaminaxNetworkAgentClass
{
  /*< private >*/
  NMSecretAgentOldClass parent_class;
};

/* used by Laminax_TYPE_NETWORK_AGENT */
GType Laminax_network_agent_get_type (void);

void               Laminax_network_agent_add_vpn_secret (LaminaxNetworkAgent *self,
                                                          gchar                *request_id,
                                                          gchar                *setting_key,
                                                          gchar                *setting_value);
void               Laminax_network_agent_set_password (LaminaxNetworkAgent *self,
                                                        gchar                *request_id,
                                                        gchar                *setting_key,
                                                        gchar                *setting_value);
void               Laminax_network_agent_respond      (LaminaxNetworkAgent        *self,
                                                        gchar                       *request_id,
                                                        LaminaxNetworkAgentResponse response);

void               Laminax_network_agent_search_vpn_plugin (LaminaxNetworkAgent *self,
                                                             const char           *service,
                                                             GAsyncReadyCallback   callback,
                                                             gpointer              user_data);
NMVpnPluginInfo   *Laminax_network_agent_search_vpn_plugin_finish (LaminaxNetworkAgent  *self,
                                                                    GAsyncResult          *result,
                                                                    GError               **error);

/* If these are kept in sync with nm-applet, secrets will be shared */
#define Laminax_KEYRING_UUID_TAG "connection-uuid"
#define Laminax_KEYRING_SN_TAG "setting-name"
#define Laminax_KEYRING_SK_TAG "setting-key"

G_END_DECLS

#endif /* __Laminax_NETWORK_AGENT_H__ */
