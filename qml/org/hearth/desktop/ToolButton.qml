import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

// Toolbar button (grove's GButton): flat, a rounded hover/pressed fill.
T.ToolButton {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: 6
    horizontalPadding: c.text !== "" && c.display !== T.AbstractButton.IconOnly ? 10 : 6
    spacing: 6
    icon.width: 16
    icon.height: 16
    icon.color: textColor
    readonly property color textColor: !c.enabled ? HearthTheme.fgDisabled : HearthTheme.fg
    contentItem: IconLabel {
        spacing: c.spacing; mirrored: c.mirrored; display: c.display
        icon: c.icon; text: c.text; font: c.font; color: c.textColor
    }
    background: Rectangle {
        implicitWidth: HearthTheme.controlHeight
        implicitHeight: HearthTheme.controlHeight
        radius: HearthTheme.radius
        // Flat like grove's toolbar buttons: a fill only when hovered,
        // pressed or checked.
        color: c.down || c.checked ? HearthTheme.pressed : c.hovered ? HearthTheme.hover : "transparent"
        border.width: c.visualFocus ? 2 : 0
        border.color: HearthTheme.alpha(HearthTheme.accent, 0.6)
    }
}
