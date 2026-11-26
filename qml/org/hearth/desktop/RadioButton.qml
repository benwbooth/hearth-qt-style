import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.RadioButton {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding, implicitIndicatorHeight + topPadding + bottomPadding)
    padding: 4
    spacing: 8
    indicator: Rectangle {
        implicitWidth: 16
        implicitHeight: 16
        x: c.text ? (c.mirrored ? c.width - width - c.rightPadding : c.leftPadding) : c.leftPadding + (c.availableWidth - width) / 2
        y: c.topPadding + (c.availableHeight - height) / 2
        radius: 8
        color: c.checked ? (c.down ? Qt.darker(HearthTheme.accent, 1.15) : HearthTheme.accent)
             : c.down ? HearthTheme.pressed : HearthTheme.textBase
        border.width: c.checked ? 0 : 1
        border.color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.16) : Qt.rgba(0, 0, 0, 0.24)
        opacity: c.enabled ? 1 : 0.45
        Rectangle { anchors.centerIn: parent; width: 6; height: 6; radius: 3; color: "#ffffff"; visible: c.checked }
    }
    contentItem: CheckLabel {
        leftPadding: c.indicator && !c.mirrored ? c.indicator.width + c.spacing : 0
        rightPadding: c.indicator && c.mirrored ? c.indicator.width + c.spacing : 0
        text: c.text
        font: c.font
        color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    }
}
