import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.ScrollIndicator {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: 2
    contentItem: Rectangle {
        implicitWidth: 5
        implicitHeight: 5
        radius: Math.min(width, height) / 2
        color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.45) : Qt.rgba(0, 0, 0, 0.4)
        visible: c.size < 1
        opacity: c.active ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: c.active ? 80 : 320 } }
    }
}
