import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

// grove's GScrollBar: slim, rounded, shown while scrolling or hovered.
T.ScrollBar {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: 2
    minimumSize: 0.04
    hoverEnabled: true
    visible: c.policy !== T.ScrollBar.AlwaysOff && c.size < 1
    contentItem: Rectangle {
        implicitWidth: c.interactive ? (c.hovered || c.pressed ? 11 : 7) : 4
        implicitHeight: c.interactive ? (c.hovered || c.pressed ? 11 : 7) : 4
        radius: Math.min(width, height) / 2
        color: HearthTheme.dark ? Qt.rgba(1, 1, 1, c.pressed ? 0.62 : 0.45) : Qt.rgba(0, 0, 0, c.pressed ? 0.55 : 0.4)
        opacity: c.policy === T.ScrollBar.AlwaysOn || (c.active && c.size < 1) ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: c.active ? 80 : 320 } }
        Behavior on implicitWidth { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
        Behavior on implicitHeight { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
    }
    background: Rectangle {
        radius: Math.min(width, height) / 2
        color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.06) : Qt.rgba(0, 0, 0, 0.04)
        border.color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.08) : Qt.rgba(0, 0, 0, 0.06)
        border.width: 1
        opacity: c.hovered || c.pressed ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: 120 } }
    }
}
