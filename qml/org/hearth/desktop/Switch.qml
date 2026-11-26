import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.Switch {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding, implicitIndicatorHeight + topPadding + bottomPadding)
    padding: 4
    spacing: 8
    indicator: Rectangle {
        implicitWidth: 34
        implicitHeight: 20
        x: c.text ? (c.mirrored ? c.leftPadding : c.width - width - c.rightPadding) : c.leftPadding + (c.availableWidth - width) / 2
        y: c.topPadding + (c.availableHeight - height) / 2
        radius: 10
        color: c.checked ? HearthTheme.accent : HearthTheme.field
        border.width: c.checked ? 0 : 1
        border.color: HearthTheme.separator
        opacity: c.enabled ? 1 : 0.45
        Behavior on color { ColorAnimation { duration: 120 } }
        Rectangle {
            x: Math.max(2, Math.min(parent.width - width - 2, c.visualPosition * parent.width - width / 2))
            y: 2
            width: 16; height: 16; radius: 8
            color: "#ffffff"
            Behavior on x { enabled: !c.down; NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
        }
    }
    contentItem: CheckLabel {
        leftPadding: c.indicator && c.mirrored ? c.indicator.width + c.spacing : 0
        rightPadding: c.indicator && !c.mirrored ? c.indicator.width + c.spacing : 0
        text: c.text
        font: c.font
        color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    }
}
