import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.Popup {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    padding: 12
    background: Rectangle {
        radius: HearthTheme.glassRadius
        color: HearthTheme.glass
        border.color: HearthTheme.glassEdge
        border.width: 1
    }
    T.Overlay.modal: Rectangle { color: Qt.rgba(0, 0, 0, 0.3) }
    T.Overlay.modeless: Rectangle { color: "transparent" }
}
