pragma Singleton
import QtQuick

// The look of Ben's own Qt Quick apps (grove's Theme.qml, hearth's
// MacMenuStyle.qml), for every Qt Quick Controls app: light/dark and the
// accent follow the application palette (hearth writes it into qt6ct and
// kdeglobals). 1 pt = 1.125 logical px.
QtObject {
    readonly property SystemPalette pal: SystemPalette { colorGroup: SystemPalette.Active }
    readonly property bool dark: pal.window.hslLightness < 0.5
    readonly property real pt: 1.125

    readonly property color accent: pal.highlight
    readonly property color accentText: "#ffffff"
    readonly property color fg: dark ? "#f5f5f7" : "#1d1d1f"
    readonly property color fgSecondary: dark ? Qt.rgba(1, 1, 1, 0.55) : Qt.rgba(0, 0, 0, 0.5)
    readonly property color fgTertiary: dark ? Qt.rgba(1, 1, 1, 0.32) : Qt.rgba(0, 0, 0, 0.28)
    readonly property color fgDisabled: dark ? Qt.rgba(1, 1, 1, 0.28) : Qt.rgba(0, 0, 0, 0.26)
    readonly property color separator: dark ? Qt.rgba(1, 1, 1, 0.10) : Qt.rgba(0, 0, 0, 0.09)
    readonly property color hover: dark ? Qt.rgba(1, 1, 1, 0.07) : Qt.rgba(0, 0, 0, 0.05)
    readonly property color pressed: dark ? Qt.rgba(1, 1, 1, 0.14) : Qt.rgba(0, 0, 0, 0.10)
    readonly property color selectedInactive: dark ? Qt.rgba(1, 1, 1, 0.13) : Qt.rgba(0, 0, 0, 0.08)
    readonly property color field: dark ? Qt.rgba(1, 1, 1, 0.08) : Qt.rgba(0, 0, 0, 0.05)
    readonly property color fieldHover: dark ? Qt.rgba(1, 1, 1, 0.14) : Qt.rgba(0, 0, 0, 0.09)
    readonly property color textBase: dark ? Qt.rgba(1, 1, 1, 0.07) : "#ffffff"
    readonly property color fieldBorder: dark ? Qt.rgba(1, 1, 1, 0.10) : Qt.rgba(0, 0, 0, 0.14)
    readonly property color segment: dark ? Qt.rgba(1, 1, 1, 0.22) : "#ffffff"
    readonly property color glass: dark ? Qt.rgba(0.17, 0.175, 0.19, 0.985) : Qt.rgba(0.975, 0.975, 0.98, 0.985)
    readonly property color glassEdge: dark ? Qt.rgba(0, 0, 0, 0.6) : Qt.rgba(0, 0, 0, 0.13)
    readonly property color glassRim: dark ? Qt.rgba(1, 1, 1, 0.14) : Qt.rgba(1, 1, 1, 0.85)
    readonly property color knob: dark ? "#ececee" : "#ffffff"

    readonly property int controlHeight: Math.round(28 * pt)
    readonly property int buttonHeight: Math.round(26 * pt)
    readonly property int radius: Math.round(6 * pt)
    readonly property int glassRadius: Math.round(12 * pt)
    readonly property int rowHeight: Math.round(24 * pt)
    readonly property int tooltipDelay: 800

    function alpha(c, a) { return Qt.rgba(c.r, c.g, c.b, a) }
}
