# Hearth Qt style

A Qt Widgets style (`QStyle` plugin, Qt 5 and Qt 6) that draws third-party Qt
and KDE apps like the [hearth](https://github.com/benwbooth) desktop shell and
its own Qt Quick apps: macOS 26 proportions, flat filled controls, capsule
push buttons with the accent on the default one, rounded text fields with an
accent focus ring, slim scroll bars, segmented tabs, and menus drawn like
hearth's (14 px corners, 27 px rows, accent selection, glass).

It is a `QProxyStyle` over Fusion; everything visible is drawn here. Colours
come from the application palette (hearth writes its palette into
qt5ct/qt6ct and kdeglobals). Kirigami / QtQuick Controls apps using the
`org.kde.desktop` style (qqc2-desktop-style) draw through it too.

## Use

    nix build .#default        # lib/qt-6/plugins/styles + lib/qt-5.x/plugins/styles

Put it on `QT_PLUGIN_PATH` (a NixOS system package does) and pick `Hearth`:
`style=Hearth` in qt5ct/qt6ct, `QT_STYLE_OVERRIDE=Hearth`, or
`widgetStyle=Hearth` in kdeglobals' `[KDE]` group (KDE apps outside Plasma
read that).

Menus are drawn translucent on Wayland (the compositor blurs behind them);
`HEARTH_STYLE_MENU_GLASS=0/1` overrides.

## Develop

    nix develop                 # Qt 6 (nix develop .#qt5 for Qt 5)
    cmake -S . -B build6 -G Ninja -DQT_MAJOR_VERSION=6 && ninja -C build6
    test/rig.sh -- build6/hearth-style-gallery-qt6 --shot shot.png [--menu|--combo]

`test/rig.sh` runs an app headless (Xvfb, 4/3 scale, private XDG dirs, no
session bus) with hearth's palette and the freshly built plugin.
