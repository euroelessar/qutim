import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick 2.1
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

    ContactListModel {
        id: contactListModel
    }

    ColumnLayout {
        anchors.fill: parent

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            ListView {
//                model: 1000
//                delegate: Text { text: index }
                model: contactListModel //accountManager.enabledAccounts

                delegate: Text {
                    text: displayName
                }
            }
        }

        Menu {
            id: statusMenu
            MenuItem {
                text: "Online"
                onTriggered: {}
            }

            MenuItem {
                text: "Offline"
                onTriggered: {}
            }

            MenuSeparator {}

            Instantiator {
                model: accountManager.enabledAccounts

                MenuItem {
                    text: displayName
                    onTriggered: {}
                }

                onObjectAdded: statusMenu.insertItem(index, object)
                onObjectRemoved: statusMenu.removeItem(object)
            }
        }

        Button {
            Layout.fillWidth: true
            text: "Click me"
            onClicked: statusMenu.popup()
        }
    }

//    SplitView {
//        anchors.fill: parent
//        TableView {
//            frameVisible: false
//            highlightOnFocus: false
//            model: 40
//            TableViewColumn {
//                title: "Left Column"
//            }
//        }
//        TextArea {
//            frameVisible: false
//            highlightOnFocus: false
//            text: {
//                var accounts = accountManager.enabledAccounts;
//                var text = "Hello World, accounts: " + accounts.length + "\n";
//                for (var i = 0; i < accounts.length; ++i) {
//                    var account = accounts[i];
//                    text += JSON.stringify(account) + "\n";
//                }
//                return text;
//            }
//        }
//    }
}
