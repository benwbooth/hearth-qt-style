import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.RadioDelegate {
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
        implicitWidth: 16
        implicitHeight: 16
        x: c.mirrored ? c.leftPadding : c.width - width - c.rightPadding
        y: c.topPadding + (c.availableHeight - height) / 2
        radius: 8
        color: c.checked ? (c.down ? Qt.darker(HearthTheme.accent, 1.15) : HearthTheme.accent)
             : c.down ? HearthTheme.pressed : HearthTheme.textBase
        border.width: c.checked ? 0 : 1
        border.color: HearthTheme.dark ? Qt.rgba(1, 1, 1, 0.16) : Qt.rgba(0, 0, 0, 0.24)
        opacity: c.enabled ? 1 : 0.45
        Rectangle { anchors.centerIn: parent; width: 6; height: 6; radius: 3; color: "#ffffff"; visible: c.checked }
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
