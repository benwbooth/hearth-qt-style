import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.SwitchDelegate {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding, implicitIndicatorHeight + topPadding + bottomPadding)
    padding: 6
    horizontalPadding: 14
    leftInset: 4
    rightInset: 4
    spacing: 8
    icon.width: 16
    icon.height: 16
    icon.color: !c.enabled ? HearthTheme.fgDisabled : c.highlighted ? HearthTheme.accentText : HearthTheme.fg
    indicator: Rectangle {
        implicitWidth: 34
        implicitHeight: 20
        x: c.mirrored ? c.leftPadding : c.width - width - c.rightPadding
        y: c.topPadding + (c.availableHeight - height) / 2
        radius: 10
        color: c.checked ? HearthTheme.accent : HearthTheme.field
        border.width: c.checked ? 0 : 1
        border.color: HearthTheme.separator
        opacity: c.enabled ? 1 : 0.45
        Behavior on color { ColorAnimation { duration: 120 } }
        Rectangle {
            x: Math.max(2, Math.min(parent.width - width - 2, c.visualPosition * parent.width - width / 2))
            y: 2
            width: 16; height: 16; radius: 8
            color: "#ffffff"
            Behavior on x { enabled: !c.down; NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
        }
    }
    contentItem: IconLabel {
        leftPadding: c.indicator && c.mirrored ? c.indicator.width + c.spacing : 0
        rightPadding: c.indicator && !c.mirrored ? c.indicator.width + c.spacing : 0
        spacing: c.spacing; mirrored: c.mirrored; display: c.display
        alignment: c.display === IconLabel.IconOnly || c.display === IconLabel.TextUnderIcon ? Qt.AlignCenter : Qt.AlignLeft
        icon: c.icon; text: c.text; font: c.font
        color: !c.enabled ? HearthTheme.fgDisabled : c.highlighted ? HearthTheme.accentText : HearthTheme.fg
    }
    background: Rectangle {
        implicitHeight: HearthTheme.rowHeight
        // Rows inset and rounded, as grove's lists and sidebar.
        radius: HearthTheme.radius
        color: c.highlighted ? HearthTheme.accent
             : c.down ? HearthTheme.pressed : c.hovered ? HearthTheme.hover : "transparent"
    }
}
