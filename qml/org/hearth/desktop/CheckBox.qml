import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.CheckBox {
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
        radius: 4
        readonly property bool on: c.checkState !== Qt.Unchecked
        color: on ? (c.down ? Qt.darker(HearthTheme.accent, 1.15) : HearthTheme.accent)
             : c.down ? HearthTheme.pressed : HearthTheme.textBase
        border.width: on ? 0 : 1
        border.color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.16) : Qt.rgba(0, 0, 0, 0.24)
        opacity: c.enabled ? 1 : 0.45
        // A drawn check (not a font glyph), as the Hearth widget style.
        Canvas {
            anchors.fill: parent
            visible: parent.on
            property bool partial: c.checkState === Qt.PartiallyChecked
            onPartialChanged: requestPaint()
            onVisibleChanged: requestPaint()
            onPaint: {
                const g = getContext("2d"); g.reset()
                g.strokeStyle = "#ffffff"; g.lineWidth = 1.8; g.lineCap = "round"; g.lineJoin = "round"
                g.beginPath()
                if (partial) { g.moveTo(4.5, 8); g.lineTo(11.5, 8) }
                else { g.moveTo(3.5, 8.5); g.lineTo(6.7, 11.7); g.lineTo(12.5, 4.6) }
                g.stroke()
            }
        }
        Rectangle {
            anchors.fill: parent; anchors.margins: -3; radius: 6
            color: "transparent"; visible: c.visualFocus
            border.width: 2; border.color: HearthTheme.alpha(HearthTheme.accent, 0.6)
        }
    }
    contentItem: CheckLabel {
        leftPadding: c.indicator && !c.mirrored ? c.indicator.width + c.spacing : 0
        rightPadding: c.indicator && c.mirrored ? c.indicator.width + c.spacing : 0
        text: c.text
        font: c.font
        color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    }
}
