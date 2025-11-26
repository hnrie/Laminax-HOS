#!/usr/bin/python3

from SettingsWidgets import SidePage
from xapp.GSettingsWidgets import *


class Module:
    name = "workspaces"
    category = "prefs"
    comment = _("Manage workspace preferences")

    def __init__(self, content_box):
        keywords = _("workspace, osd, expo, monitor")
        sidePage = SidePage(_("Workspaces"), "cs-workspaces", keywords, content_box, module=self)
        self.sidePage = sidePage

    def shouldLoad(self):
        return True

    def on_module_selected(self):
        if not self.loaded:
            print("Loading Workspaces module")

            page = SettingsPage()
            self.sidePage.add_widget(page)

            settings = page.add_section(_("Workspace Options"))

            switch = GSettingsSwitch(_("Enable workspace OSD"), "org.Laminax", "workspace-osd-visible")
            settings.add_row(switch)

            switch = GSettingsSwitch(_("Allow cycling through workspaces"), "org.Laminax.muffin", "workspace-cycle")
            settings.add_row(switch)

            switch = GSettingsSwitch(_("Only use workspaces on primary monitor (requires Laminax restart)"), "org.Laminax.muffin", "workspaces-only-on-primary")
            settings.add_row(switch)

            switch = GSettingsSwitch(_("Display Expo view as a grid"), "org.Laminax", "workspace-expo-view-as-grid")
            settings.add_row(switch)

            switch = GSettingsSwitch(_("Always show Expo on the primary monitor"), "org.Laminax", "workspace-expo-primary-monitor")
            settings.add_row(switch)

            # Edge Flip doesn't work well, so it's there in gsettings, but we don't show it to users yet
            # switch = GSettingsSwitch(_("Enable Edge Flip"), "org.Laminax", "enable-edge-flip")
            # settings.add_row(switch)
            # spin = GSettingsSpinButton(_("Edge Flip delay"), "org.Laminax", "edge-flip-delay", mini=1, maxi=3000, units=_("ms"))
            # settings.add_reveal_row(spin, "org.Laminax", "enable-edge-flip")

            # switch = GSettingsSwitch(_("Invert the left and right arrow key directions used to shift workspaces during a window drag"), "org.Laminax.muffin", "invert-workspace-flip-direction")
            # settings.add_row(switch)
