import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl

// Tabs as a segmented control (grove's Segmented): a capsule track, the
// current tab raised.
T.TabBar {
    id: c
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    padding: 2
    spacing: 0
    contentItem: ListView {
        model: c.contentModel
        currentIndex: c.currentIndex
        spacing: c.spacing
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.AutoFlickIfNeeded
        snapMode: ListView.SnapToItem
        highlightMoveDuration: 120
        highlightResizeDuration: 0
        highlightFollowsCurrentItem: true
        highlightRangeMode: ListView.ApplyRange
        preferredHighlightBegin: 40
        preferredHighlightEnd: width - 40
        highlight: Rectangle {
            radius: HearthTheme.radius - 2
            color: HearthTheme.segment
            border.width: HearthTheme.dark ? 0 : 1
            border.color: Qt.rgba(0, 0, 0, 0.08)
        }
    }
    background: Rectangle {
        implicitHeight: HearthTheme.controlHeight
        radius: HearthTheme.radius
        color: HearthTheme.field
    }
}
