#!/usr/bin/env python3
"""A stand-in FileChooser portal for tests (test/rig.sh dialogs): owns
org.freedesktop.portal.Desktop on a private bus, logs each OpenFile/SaveFile
call as a JSON line on stdout and answers "cancelled"."""
import json, sys
import dbus, dbus.service
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

DBusGMainLoop(set_as_default=True)
bus = dbus.SessionBus()
name = dbus.service.BusName("org.freedesktop.portal.Desktop", bus)
IFACE = "org.freedesktop.portal.FileChooser"

class Request(dbus.service.Object):
    @dbus.service.signal("org.freedesktop.portal.Request", signature="ua{sv}")
    def Response(self, code, results):
        pass

class Portal(dbus.service.Object):
    def _call(self, kind, parent, title, options, sender):
        print(json.dumps({"call": kind, "title": str(title), "parent": str(parent),
                          "options": sorted(str(k) for k in options)}), flush=True)
        token = str(options.get("handle_token", "t"))
        path = "/org/freedesktop/portal/desktop/request/%s/%s" % (sender[1:].replace(".", "_"), token)
        req = Request(bus, path)
        GLib.timeout_add(50, lambda: (req.Response(dbus.UInt32(1), dbus.Dictionary({}, signature="sv")), False)[1])
        return dbus.ObjectPath(path)

    @dbus.service.method(IFACE, in_signature="ssa{sv}", out_signature="o", sender_keyword="sender")
    def OpenFile(self, parent, title, options, sender=None):
        return self._call("OpenFile", parent, title, options, sender)

    @dbus.service.method(IFACE, in_signature="ssa{sv}", out_signature="o", sender_keyword="sender")
    def SaveFile(self, parent, title, options, sender=None):
        return self._call("SaveFile", parent, title, options, sender)

    @dbus.service.method(dbus.PROPERTIES_IFACE, in_signature="ss", out_signature="v")
    def Get(self, iface, prop):
        if prop == "version":
            return dbus.UInt32(4)
        raise dbus.exceptions.DBusException("org.freedesktop.DBus.Error.UnknownProperty", prop)

    @dbus.service.method(dbus.PROPERTIES_IFACE, in_signature="s", out_signature="a{sv}")
    def GetAll(self, iface):
        return dbus.Dictionary({"version": dbus.UInt32(4)}, signature="sv")

Portal(bus, "/org/freedesktop/portal/desktop")
print(json.dumps({"ready": True}), flush=True)
GLib.MainLoop().run()
