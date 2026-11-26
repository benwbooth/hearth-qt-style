import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.ToolSeparator {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    padding: c.vertical ? 6 : 2
    verticalPadding: c.vertical ? 4 : 6
    contentItem: Rectangle {
        implicitWidth: c.vertical ? 1 : 20
        implicitHeight: c.vertical ? 20 : 1
        color: HearthTheme.separator
    }
}
