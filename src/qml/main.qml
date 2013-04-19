import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick 2.1

import org.qutim.core 0.4
import org.qutim.icons 0.4

import "."

import "settings"

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
            id: contactListView

            Layout.fillWidth: true
            Layout.fillHeight: true
            frameVisible: true

            ListView {
                //                model: 1000
                //                delegate: Text { text: index }

                highlightFollowsCurrentItem: true
                model: contactListModel
                focus: true

                signal activated

                SystemPalette {
                    id: palette
                    colorGroup: ListView.enabled ? SystemPalette.Active : SystemPalette.Disabled
                }

                MouseArea {
                    id: mousearea

                    anchors.fill: listView

                    preventStealing: true


                    onPressed:  {
                        listView.forceActiveFocus()
                        var x = listView.contentX + mouseX;
                        var y = listView.contentY + mouseY;
                        listView.currentIndex = listView.indexAt(x, y);
                    }

                    onDoubleClicked: listView.activated()
                }

                delegate: ContactItem {}

                Keys.onUpPressed: listView.decrementCurrentIndex()
                Keys.onDownPressed: listView.incrementCurrentIndex()
                Keys.onReturnPressed: listView.activated()

                onActivated: {
                    console.log("clicked on " + listView.currentIndex)
                }
            }
        }

        Menu {
            id: statusMenu

            MenuItem {
                text: qsTr("Settings")
                onTriggered: settingsDialog.show()
            }

            MenuItem {
                text: "Online"
                onTriggered: {}
            }

            MenuItem {
                text: "Offline"
                onTriggered: {}
            }

            MenuSeparator {
                visible: accountManager.enabledAccounts.length > 0
            }

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

    SettingsDialog {
        id: settingsDialog

        items: [
            SettingsItem {
                title: qsTr("Foo")
                iconName: "applications-internet"

                Column {
                    anchors.centerIn: parent

                    Text {
                        text: qsTr("Text me")
                    }

                    Image {
                        source: "image://icontheme/applications-internet"
                        width: 64
                        height: 64
                    }

                }
            },

            SettingsItem {
                title: qsTr("Boo")
                iconName: "applications-system"

                Rectangle {
                    anchors.fill: parent
                    color: systemPalette.base

                    ScrollView {
                        anchors.fill: parent
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Text {
                            text: "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor "+
                                  "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor "+
                                  "incididunt ut labore et dolore magna aliqua.\n Ut enim ad minim veniam, quis nostrud "+
                                  "exercitation ullamco laboris nisi ut aliquip ex ea commodo cosnsequat. ";
                        }
                    }
                }
            }
        ]
    }

    SystemPalette {
        id: systemPalette
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
