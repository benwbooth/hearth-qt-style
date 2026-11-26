// Qt Quick Controls gallery for the org.hearth.desktop style:
//   QT_QUICK_CONTROLS_STYLE=org.hearth.desktop qml test/gallery.qml
// Its controls use the plain QtQuick.Controls import, so the style decides.
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: win
    width: 900; height: 600; visible: true
    title: "org.hearth.desktop gallery"
    property string open: Qt.application.arguments.indexOf("--menu") >= 0 ? "menu"
                        : Qt.application.arguments.indexOf("--combo") >= 0 ? "combo" : ""
    menuBar: MenuBar {
        Menu {
            id: fileMenu
            title: "&File"
            Action { text: "New Window"; shortcut: "Ctrl+N"; icon.name: "document-new" }
            Action { text: "Open…"; shortcut: "Ctrl+O"; icon.name: "document-open" }
            Menu { title: "Open Recent"; Action { text: "notes.txt" } }
            MenuSeparator {}
            Action { text: "Show Sidebar"; checkable: true; checked: true; shortcut: "Ctrl+Alt+S" }
            Action { text: "Save"; shortcut: "Ctrl+S" }
            Action { text: "Revert to Saved"; enabled: false }
            MenuSeparator {}
            Action { text: "Close Window"; shortcut: "Ctrl+W" }
        }
        Menu { title: "&Edit" }
        Menu { title: "&View" }
    }
    header: ToolBar {
        RowLayout {
            ToolButton { icon.name: "go-previous"; text: "Back"; display: AbstractButton.IconOnly }
            ToolButton { icon.name: "go-next"; text: "Forward"; display: AbstractButton.IconOnly }
            ToolSeparator {}
            ToolButton { text: "Grid"; checkable: true; checked: true }
            ToolButton { text: "List" }
        }
    }
    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16
        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredWidth: 3
            spacing: 10
            RowLayout {
                Button { text: "Cancel" }
                Button { text: "OK"; highlighted: true }
                Button { text: "Disabled"; enabled: false }
            }
            GridLayout {
                columns: 2
                columnSpacing: 10
                Label { text: "Name:"; Layout.alignment: Qt.AlignRight }
                TextField { id: name; text: "Hearth"; Layout.fillWidth: true }
                Label { text: "Search:"; Layout.alignment: Qt.AlignRight }
                TextField { placeholderText: "Search"; Layout.fillWidth: true }
                Label { text: "Appearance:"; Layout.alignment: Qt.AlignRight }
                ComboBox { id: combo; model: ["Automatic", "Light", "Dark"]; currentIndex: 2; Layout.fillWidth: true }
                Label { text: "Font:"; Layout.alignment: Qt.AlignRight }
                ComboBox { editable: true; model: ["Inter", "Noto Sans"]; Layout.fillWidth: true }
                Label { text: "Size:"; Layout.alignment: Qt.AlignRight }
                SpinBox { from: 6; to: 72; value: 13; Layout.fillWidth: true }
                Label { text: "Glass:"; Layout.alignment: Qt.AlignRight }
                Slider { value: 0.62; Layout.fillWidth: true }
                Label { text: "Progress:"; Layout.alignment: Qt.AlignRight }
                ProgressBar { value: 0.4; Layout.fillWidth: true }
            }
            RowLayout {
                CheckBox { text: "Show hidden"; checked: true }
                CheckBox { text: "Previews" }
                RadioButton { text: "Icons"; checked: true }
                RadioButton { text: "List" }
                Switch { text: "Sync"; checked: true }
            }
            TabBar {
                Layout.alignment: Qt.AlignHCenter
                TabButton { text: "General"; width: implicitWidth }
                TabButton { text: "Tags"; width: implicitWidth }
                TabButton { text: "Advanced"; width: implicitWidth }
            }
            GroupBox {
                title: "Sidebar"
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    CheckBox { text: "Recents" }
                    CheckBox { text: "Tags"; tristate: true; checkState: Qt.PartiallyChecked }
                }
            }
        }
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 2
            padding: 0
            ListView {
                id: list
                anchors.fill: parent
                anchors.margins: 4
                clip: true
                model: ["Applications", "Desktop", "Documents", "Downloads", "Music", "Pictures", "Videos", "src", "Games", "Projects", "Backups", "Archive", "More", "And more"]
                currentIndex: 2
                delegate: ItemDelegate {
                    required property string modelData
                    required property int index
                    width: ListView.view.width
                    text: modelData
                    icon.name: "folder"
                    highlighted: ListView.isCurrentItem
                    onClicked: list.currentIndex = index
                }
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AlwaysOn }
            }
        }
    }
    footer: Label { text: "  Qt Quick Controls style: org.hearth.desktop"; padding: 6 }
    Timer {
        interval: 400; running: true
        onTriggered: {
            name.forceActiveFocus()
            if (win.open === "menu") { fileMenu.popup(300 - 0, 10); fileMenu.currentIndex = 1 }
            else if (win.open === "combo") combo.popup.open()
        }
    }
}
