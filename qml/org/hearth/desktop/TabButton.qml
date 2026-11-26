import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.TabButton {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: 4
    horizontalPadding: 14
    spacing: 6
    icon.width: 16
    icon.height: 16
    icon.color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    contentItem: IconLabel {
        spacing: c.spacing; mirrored: c.mirrored; display: c.display
        icon: c.icon; text: c.text; font: c.font
        color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    }
    background: Rectangle {
        implicitHeight: HearthTheme.controlHeight - 4
        radius: HearthTheme.radius - 2
        color: !c.checked && (c.down || c.hovered) ? HearthTheme.hover : "transparent"
    }
}
