/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 20011 Red Hat, Inc.
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#pragma once

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE
#include <polkitagent/polkitagent.h>
#include <glib-object.h>

G_BEGIN_DECLS

#if !(POLKIT_VERSION >= 121)
/* Polkit < 121 doesn't have g_autoptr support */
G_DEFINE_AUTOPTR_CLEANUP_FUNC (PolkitAgentListener, g_object_unref)
#endif

#define Laminax_TYPE_POLKIT_AUTHENTICATION_AGENT (Laminax_polkit_authentication_agent_get_type())

G_DECLARE_FINAL_TYPE (LaminaxPolkitAuthenticationAgent, Laminax_polkit_authentication_agent, Laminax, POLKIT_AUTHENTICATION_AGENT, PolkitAgentListener)

LaminaxPolkitAuthenticationAgent *Laminax_polkit_authentication_agent_new (void);

void                            Laminax_polkit_authentication_agent_complete (LaminaxPolkitAuthenticationAgent *agent,
                                                                            gboolean                        dismissed);
gchar *                         Laminax_polkit_authentication_agent_register (LaminaxPolkitAuthenticationAgent *agent,
                                                                            GError                        **error_out);
void                            Laminax_polkit_authentication_agent_unregister (LaminaxPolkitAuthenticationAgent *agent);

G_END_DECLS

