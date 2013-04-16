import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import org.qutim.core 0.4


Window {
    width: 250
    height: 600
    minimumWidth: 100
    minimumHeight: 300

//    menuBar: MenuBar {
//        Menu {
//            title: "File"
//            MenuItem { text: "Quit"; onTriggered: Qt.quit() }
//        }
//        Menu {
//            title: "Help"
//            MenuItem { text: "About..."; enabled: false }
//        }
//    }

//    toolBar: ToolBar {
//        RowLayout {
//            ToolButton {
//                iconName: "image://desktoptheme/insert-text"
//            }
//            ToolButton { iconSource: "image://desktoptheme/file-menu" }
//        }
//    }

    AccountManager {
        id: accountManager
    }

    SplitView {
        anchors.fill: parent
        TableView {
            frameVisible: false
            highlightOnFocus: false
            model: 40
            TableViewColumn {
                title: "Left Column"
            }
        }
        TextArea {
            frameVisible: false
            highlightOnFocus: false
            text: "Hello World, accounts: " + accountManager.allAccounts.length
        }
    }
}
