import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.ProgressBar {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    contentItem: Item {
        implicitWidth: 200
        implicitHeight: 6
        Rectangle {
            width: c.indeterminate ? parent.width * 0.3 : c.visualPosition * parent.width
            height: parent.height
            radius: 3
            color: HearthTheme.accent
            opacity: c.enabled ? 1 : 0.45
            SequentialAnimation on x {
                running: c.indeterminate && c.visible
                loops: Animation.Infinite
                NumberAnimation { from: 0; to: c.availableWidth * 0.7; duration: 900; easing.type: Easing.InOutQuad }
                NumberAnimation { from: c.availableWidth * 0.7; to: 0; duration: 900; easing.type: Easing.InOutQuad }
            }
        }
    }
    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 6
        y: (c.height - height) / 2
        height: 6
        radius: 3
        color: HearthTheme.field
    }
}
