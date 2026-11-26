import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.RoundButton {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: 6
    spacing: 6
    icon.width: 16
    icon.height: 16
    icon.color: textColor
    readonly property color textColor: !c.enabled ? HearthTheme.fgDisabled : c.highlighted ? HearthTheme.accentText : HearthTheme.fg
    contentItem: IconLabel {
        spacing: c.spacing; mirrored: c.mirrored; display: c.display
        icon: c.icon; text: c.text; font: c.font; color: c.textColor
    }
    background: Rectangle {
        implicitWidth: HearthTheme.buttonHeight
        implicitHeight: HearthTheme.buttonHeight
        radius: c.radius
        visible: !c.flat || c.down || c.hovered || c.checked
        color: c.highlighted ? HearthTheme.accent : c.down || c.checked ? HearthTheme.pressed
             : c.hovered ? HearthTheme.fieldHover : c.flat ? "transparent" : HearthTheme.field
    }
}
