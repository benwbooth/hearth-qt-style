import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

// A thin track filled with the accent to a white knob (hearth's ValueSlider).
T.Slider {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitHandleHeight + topPadding + bottomPadding)
    padding: 2
    handle: Rectangle {
        x: c.leftPadding + (c.horizontal ? c.visualPosition * (c.availableWidth - width) : (c.availableWidth - width) / 2)
        y: c.topPadding + (c.horizontal ? (c.availableHeight - height) / 2 : c.visualPosition * (c.availableHeight - height))
        implicitWidth: 18; implicitHeight: 18
        radius: 9
        color: c.pressed ? Qt.darker(HearthTheme.knob, 1.08) : HearthTheme.knob
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, HearthTheme.dark ? 0.35 : 0.14)
        Rectangle {
            anchors.fill: parent; anchors.margins: -3; radius: width / 2
            color: "transparent"; visible: c.visualFocus
            border.width: 2; border.color: HearthTheme.alpha(HearthTheme.accent, 0.6)
        }
    }
    background: Item {
        x: c.leftPadding + (c.horizontal ? 0 : (c.availableWidth - width) / 2)
        y: c.topPadding + (c.horizontal ? (c.availableHeight - height) / 2 : 0)
        implicitWidth: c.horizontal ? 200 : 20
        implicitHeight: c.horizontal ? 20 : 200
        width: c.horizontal ? c.availableWidth : 4
        height: c.horizontal ? 4 : c.availableHeight
        opacity: c.enabled ? 1 : 0.45
        Rectangle { anchors.fill: parent; radius: 2; color: HearthTheme.field }
        Rectangle {
            x: c.horizontal ? (c.mirrored ? parent.width - width : 0) : 0
            y: c.horizontal ? 0 : c.visualPosition * parent.height
            width: c.horizontal ? c.position * parent.width : parent.width
            height: c.horizontal ? parent.height : c.position * parent.height
            radius: 2
            color: HearthTheme.accent
        }
    }
}
