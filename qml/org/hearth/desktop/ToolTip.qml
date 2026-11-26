import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

// grove's GTip: glass, small text.
T.ToolTip {
    id: c
    x: parent ? (parent.width - implicitWidth) / 2 : 0
    y: -implicitHeight - 6
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    margins: 6
    padding: 4
    horizontalPadding: 8
    delay: HearthTheme.tooltipDelay
    closePolicy: T.Popup.CloseOnEscape | T.Popup.CloseOnPressOutsideParent | T.Popup.CloseOnReleaseOutsideParent
    contentItem: Text {
        text: c.text
        font: c.font
        wrapMode: Text.Wrap
        color: HearthTheme.fg
    }
    background: Rectangle {
        radius: 7
        color: HearthTheme.glass
        border.color: HearthTheme.glassEdge
        border.width: 1
    }
    enter: Transition { NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 110 } }
    exit: Transition { NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 160 } }
}
