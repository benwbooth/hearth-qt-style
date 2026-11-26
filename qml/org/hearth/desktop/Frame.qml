import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.Frame {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    padding: 12
    background: Rectangle {
        radius: 10
        color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.035) : Qt.rgba(0, 0, 0, 0.025)
        border.color: HearthTheme.separator
    }
}
