#!/usr/bin/python3

import os
import gettext
from mintcommon import additionalfiles

DOMAIN = "Laminax"
PATH = "/usr/share/locale"

os.environ['LANGUAGE'] = "en_US.UTF-8"
gettext.install(DOMAIN, PATH)

prefix = """[Desktop Entry]
Exec=env WEBKIT_DISABLE_COMPOSITING_MODE=1 Laminax-settings
Icon=preferences-desktop
Terminal=false
Type=Application
Categories=Settings;
StartupNotify=false
OnlyShowIn=X-Laminax;
Keywords=Preferences;Settings;
"""

additionalfiles.generate(DOMAIN, PATH, "files/usr/share/applications/Laminax-settings.desktop", prefix, _("System Settings"), _("Control Center"), "")

prefix = """[Desktop Entry]
Exec=Laminax-settings-users
Icon=system-users
Terminal=false
Type=Application
Categories=System;Settings;
StartupNotify=false
OnlyShowIn=X-Laminax;
Keywords=Preferences;Settings;
"""

additionalfiles.generate(DOMAIN, PATH, "files/usr/share/applications/Laminax-settings-users.desktop", prefix, _("Users and Groups"), _("Add or remove users and groups"), "")

prefix = """[Desktop Entry]
Exec=dbus-send --print-reply --dest=org.Laminax /org/Laminax org.Laminax.ToggleKeyboard
Icon=Laminax-virtual-keyboard
Terminal=false
Type=Application
Categories=Utility;
OnlyShowIn=X-Laminax;
Keywords=onboard;keyboard;caribou;
"""

additionalfiles.generate(DOMAIN, PATH, "files/usr/share/applications/Laminax-onscreen-keyboard.desktop", prefix, _("Virtual keyboard"), _("Turn on-screen keyboard on or off"), "")
