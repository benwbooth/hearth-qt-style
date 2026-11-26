import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.SpinBox {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    leftPadding: c.mirrored ? 22 : 10
    rightPadding: c.mirrored ? 10 : 22
    editable: true
    validator: IntValidator {
        locale: c.locale.name
        bottom: Math.min(c.from, c.to)
        top: Math.max(c.from, c.to)
    }
    contentItem: TextInput {
        text: c.displayText
        font: c.font
        color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
        selectionColor: HearthTheme.accent
        selectedTextColor: HearthTheme.accentText
        horizontalAlignment: Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter
        readOnly: !c.editable
        validator: c.validator
        inputMethodHints: c.inputMethodHints
        clip: width < implicitWidth
    }
    up.indicator: Item {
        x: c.mirrored ? 0 : c.width - width
        width: 22; height: c.height / 2
        Canvas {
            anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 1
            width: 8; height: 5
            property color tint: c.enabled && c.value < c.to ? (c.up.pressed ? HearthTheme.fg : HearthTheme.fgSecondary) : HearthTheme.fgDisabled
            onTintChanged: requestPaint()
            onPaint: { const g = getContext("2d"); g.reset(); g.strokeStyle = tint; g.lineWidth = 1.4; g.lineCap = "round"; g.lineJoin = "round"; g.beginPath(); g.moveTo(1, 4); g.lineTo(4, 1); g.lineTo(7, 4); g.stroke() }
        }
    }
    down.indicator: Item {
        x: c.mirrored ? 0 : c.width - width
        y: c.height / 2
        width: 22; height: c.height / 2
        Canvas {
            anchors.horizontalCenter: parent.horizontalCenter; anchors.top: parent.top; anchors.topMargin: 1
            width: 8; height: 5
            property color tint: c.enabled && c.value > c.from ? (c.down.pressed ? HearthTheme.fg : HearthTheme.fgSecondary) : HearthTheme.fgDisabled
            onTintChanged: requestPaint()
            onPaint: { const g = getContext("2d"); g.reset(); g.strokeStyle = tint; g.lineWidth = 1.4; g.lineCap = "round"; g.lineJoin = "round"; g.beginPath(); g.moveTo(1, 1); g.lineTo(4, 4); g.lineTo(7, 1); g.stroke() }
        }
    }
    background: Rectangle {
        implicitWidth: 100
        implicitHeight: HearthTheme.controlHeight
        radius: HearthTheme.radius
        color: HearthTheme.textBase
        opacity: c.enabled ? 1 : 0.5
        border.width: c.activeFocus ? 2 : 1
        border.color: c.activeFocus ? HearthTheme.alpha(HearthTheme.accent, 0.75) : HearthTheme.fieldBorder
    }
}
