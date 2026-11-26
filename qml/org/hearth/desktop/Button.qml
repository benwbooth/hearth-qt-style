import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

// Push button (grove's PushButton): a capsule, the accent when highlighted.
T.Button {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: 6
    horizontalPadding: c.text !== "" ? 14 : 6
    spacing: 6
    icon.width: 16
    icon.height: 16
    icon.color: textColor
    readonly property bool accented: c.highlighted || (c.checked && !c.flat)
    readonly property color textColor: !c.enabled ? HearthTheme.fgDisabled : c.accented ? HearthTheme.accentText : HearthTheme.fg
    contentItem: IconLabel {
        spacing: c.spacing; mirrored: c.mirrored; display: c.display
        icon: c.icon; text: c.text; font: c.font; color: c.textColor
    }
    background: Rectangle {
        implicitWidth: c.flat || c.text === "" ? HearthTheme.buttonHeight : Math.round(76 * HearthTheme.pt)
        implicitHeight: HearthTheme.buttonHeight
        radius: height / 2
        visible: !c.flat || c.down || c.hovered || c.checked || c.visualFocus
        color: c.accented ? (c.down ? Qt.darker(HearthTheme.accent, 1.15) : HearthTheme.accent)
             : c.down ? HearthTheme.pressed : c.hovered ? HearthTheme.fieldHover
             : c.flat ? "transparent" : HearthTheme.field
        opacity: c.enabled ? 1 : 0.5
        border.width: c.visualFocus ? 2 : 0
        border.color: HearthTheme.alpha(HearthTheme.accent, 0.6)
    }
}
