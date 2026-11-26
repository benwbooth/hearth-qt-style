import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl
import QtQuick.Controls.Basic as B
import "impl"

// Pop-up button (hearth's MacComboBox): the glass list opens with the
// current choice over the button. Editable ones are a field with the list
// below. (Basic's ComboBox underneath: MacComboPopup and MacComboItem take
// one.)
B.ComboBox {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding, implicitIndicatorHeight + topPadding + bottomPadding)
    leftPadding: c.editable ? 10 : Math.round(9 * HearthTheme.pt)
    rightPadding: Math.round(24 * HearthTheme.pt)
    implicitContentWidthPolicy: T.ComboBox.WidestText
    delegate: MacComboItem { combo: c }
    indicator: Canvas {
        id: chev
        x: c.mirrored ? 9 : c.width - width - 9
        y: (c.height - height) / 2
        width: 8; height: 12
        property color tint: c.enabled ? HearthTheme.fgSecondary : HearthTheme.fgDisabled
        onTintChanged: requestPaint()
        onPaint: {
            const g = getContext("2d"); g.reset()
            g.strokeStyle = tint; g.lineWidth = 1.4; g.lineCap = "round"; g.lineJoin = "round"
            g.beginPath(); g.moveTo(1, 4.5); g.lineTo(4, 1.5); g.lineTo(7, 4.5); g.stroke()
            g.beginPath(); g.moveTo(1, 7.5); g.lineTo(4, 10.5); g.lineTo(7, 7.5); g.stroke()
        }
    }
    contentItem: T.TextField {
        leftPadding: 0
        rightPadding: 0
        text: c.editable ? c.editText : c.displayText
        enabled: c.editable
        autoScroll: c.editable
        readOnly: c.down
        inputMethodHints: c.inputMethodHints
        validator: c.validator
        selectByMouse: c.selectTextByMouse
        font: c.font
        color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
        selectionColor: HearthTheme.accent
        selectedTextColor: HearthTheme.accentText
        verticalAlignment: Text.AlignVCenter
    }
    background: Rectangle {
        implicitWidth: Math.round(90 * HearthTheme.pt)
        implicitHeight: HearthTheme.controlHeight
        radius: HearthTheme.radius
        color: c.editable ? HearthTheme.textBase
             : HearthTheme.dark ? Qt.rgba(1, 1, 1, c.pressed ? 0.20 : c.hovered ? 0.16 : 0.12)
             : (c.pressed ? Qt.rgba(0, 0, 0, 0.06) : "#ffffff")
        border.width: c.visualFocus || (c.editable && c.activeFocus) ? 2 : 1
        border.color: c.visualFocus || (c.editable && c.activeFocus) ? HearthTheme.alpha(HearthTheme.accent, 0.75)
                    : HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.08) : Qt.rgba(0, 0, 0, 0.14)
        opacity: c.enabled ? 1 : 0.5
    }
    popup: MacComboPopup { combo: c }
}
