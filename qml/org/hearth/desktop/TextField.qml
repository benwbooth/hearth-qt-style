import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

// A text field: a rounded well with a hairline, an accent ring when focused
// (the Hearth widget style, grove's fields).
T.TextField {
    id: c
    implicitWidth: implicitBackgroundWidth + leftInset + rightInset || Math.max(contentWidth, placeholder.implicitWidth) + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding, placeholder.implicitHeight + topPadding + bottomPadding)
    padding: 6
    leftPadding: 10
    rightPadding: 10
    color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    selectionColor: HearthTheme.accent
    selectedTextColor: HearthTheme.accentText
    placeholderTextColor: HearthTheme.fgTertiary
    verticalAlignment: TextInput.AlignVCenter
    PlaceholderText {
        id: placeholder
        x: c.leftPadding; y: c.topPadding
        width: c.width - (c.leftPadding + c.rightPadding)
        height: c.height - (c.topPadding + c.bottomPadding)
        text: c.placeholderText; font: c.font; color: c.placeholderTextColor
        verticalAlignment: c.verticalAlignment
        visible: !c.length && !c.preeditText && (!c.activeFocus || c.horizontalAlignment !== Qt.AlignHCenter)
        elide: Text.ElideRight
        renderType: c.renderType
    }
    background: Rectangle {
        implicitWidth: 200
        implicitHeight: HearthTheme.controlHeight
        radius: HearthTheme.radius
        color: HearthTheme.textBase
        opacity: c.enabled ? 1 : 0.5
        border.width: c.activeFocus ? 2 : 1
        border.color: c.activeFocus ? HearthTheme.alpha(HearthTheme.accent, 0.75) : HearthTheme.fieldBorder
    }
}
