import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.GroupBox {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding, implicitLabelWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    spacing: 6
    padding: 12
    topPadding: padding + (implicitLabelWidth > 0 ? implicitLabelHeight + spacing : 0)
    label: Text {
        x: c.leftPadding - 8
        width: c.availableWidth
        text: c.title
        font: c.font
        color: c.enabled ? HearthTheme.fgSecondary : HearthTheme.fgDisabled
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
    background: Rectangle {
        y: c.topPadding - c.bottomPadding
        width: parent.width
        height: parent.height - c.topPadding + c.bottomPadding
        radius: 10
        color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.035) : Qt.rgba(0, 0, 0, 0.025)
        border.color: HearthTheme.separator
    }
}
