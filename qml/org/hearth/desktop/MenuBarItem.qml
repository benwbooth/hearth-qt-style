import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.MenuBarItem {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: 4
    horizontalPadding: 9
    spacing: 6
    icon.width: 16
    icon.height: 16
    icon.color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    // (No mnemonic underlines: macOS.)
    contentItem: IconLabel {
        spacing: c.spacing; mirrored: c.mirrored; display: c.display; alignment: Qt.AlignLeft
        icon: c.icon; text: c.text.replace(/&(?!&)/g, "").replace(/&&/g, "&"); font: c.font
        color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    }
    background: Rectangle {
        implicitHeight: Math.round(22 * HearthTheme.pt)
        radius: HearthTheme.radius
        color: c.highlighted || c.down ? HearthTheme.pressed : "transparent"
    }
}
