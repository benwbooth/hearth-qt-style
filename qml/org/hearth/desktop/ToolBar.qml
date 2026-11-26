import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.ToolBar {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    padding: 6
    background: Rectangle {
        implicitHeight: Math.round(52 * HearthTheme.pt) - 12
        color: c.palette.window
        Rectangle {
            width: parent.width; height: 1
            y: c.position === T.ToolBar.Footer ? 0 : parent.height - 1
            color: HearthTheme.separator
        }
    }
}
