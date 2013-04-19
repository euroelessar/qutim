import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick 2.1
import org.qutim.core 0.4
import "."

Window {
    width: 250
    height: 600
    minimumWidth: 100
    minimumHeight: 300

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
                id: listView

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
}
