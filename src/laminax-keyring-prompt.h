/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* Laminax-keyring-prompt.c - prompt handler for gnome-keyring-daemon

   Copyright (C) 2011 Stefan Walter

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Author: Stef Walter <stef@thewalter.net>
*/

#ifndef __Laminax_KEYRING_PROMPT_H__
#define __Laminax_KEYRING_PROMPT_H__

#include <glib-object.h>
#include <glib.h>

#include <clutter/clutter.h>

G_BEGIN_DECLS

typedef struct _LaminaxKeyringPrompt LaminaxKeyringPrompt;

#define Laminax_TYPE_KEYRING_PROMPT (Laminax_keyring_prompt_get_type ())
G_DECLARE_FINAL_TYPE (LaminaxKeyringPrompt, Laminax_keyring_prompt,
                      Laminax, KEYRING_PROMPT, GObject)

LaminaxKeyringPrompt * Laminax_keyring_prompt_new                  (void);

ClutterText *           Laminax_keyring_prompt_get_password_actor   (LaminaxKeyringPrompt *self);

void                    Laminax_keyring_prompt_set_password_actor   (LaminaxKeyringPrompt *self,
                                                                      ClutterText           *password_actor);

ClutterText *           Laminax_keyring_prompt_get_confirm_actor    (LaminaxKeyringPrompt *self);

void                    Laminax_keyring_prompt_set_confirm_actor    (LaminaxKeyringPrompt *self,
                                                                      ClutterText           *confirm_actor);

gboolean                Laminax_keyring_prompt_complete             (LaminaxKeyringPrompt *self);

void                    Laminax_keyring_prompt_cancel               (LaminaxKeyringPrompt *self);

G_END_DECLS

#endif /* __Laminax_KEYRING_PROMPT_H__ */

