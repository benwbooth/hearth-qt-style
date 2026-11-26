import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.MenuBar {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    padding: 2
    leftPadding: 6
    delegate: MenuBarItem {}
    contentItem: Row {
        spacing: c.spacing
        Repeater { model: c.contentModel }
    }
    background: Rectangle { color: c.palette.window }
}
