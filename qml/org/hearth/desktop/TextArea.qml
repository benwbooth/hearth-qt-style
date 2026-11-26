import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.TextArea {
    id: c
    implicitWidth: Math.max(contentWidth + leftPadding + rightPadding, implicitBackgroundWidth + leftInset + rightInset, placeholder.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(contentHeight + topPadding + bottomPadding, implicitBackgroundHeight + topInset + bottomInset, placeholder.implicitHeight + topPadding + bottomPadding)
    padding: 8
    leftPadding: 10
    rightPadding: 10
    color: c.enabled ? HearthTheme.fg : HearthTheme.fgDisabled
    selectionColor: HearthTheme.accent
    selectedTextColor: HearthTheme.accentText
    placeholderTextColor: HearthTheme.fgTertiary
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
        implicitHeight: HearthTheme.controlHeight * 2
        radius: HearthTheme.radius
        color: HearthTheme.textBase
        border.width: c.activeFocus ? 2 : 1
        border.color: c.activeFocus ? HearthTheme.alpha(HearthTheme.accent, 0.75) : HearthTheme.fieldBorder
    }
}
