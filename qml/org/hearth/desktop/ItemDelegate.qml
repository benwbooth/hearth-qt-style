import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

T.ItemDelegate {
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
    
    contentItem: IconLabel {
        
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
