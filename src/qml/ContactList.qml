import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick 2.1
import org.qutim.core 0.4

Window {
    id: root
    width: 250
    height: 600
    minimumWidth: 100
    minimumHeight: 300

    signal settingsRequested

    Config {
        id: config
        path: "desktop.contactList"
        property string theme: "default"
        property bool showOffline: false
        property int value: 0
    }

    AccountManager {
        id: accountManager
    }

    ContactListModel {
        id: contactListModel

        showOffline: config.showOffline
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

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
            }

            MenuItem {
                text: "Show settings"
                onTriggered: root.settingsRequested()
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

//        Row {
//            id: accountsRow
//            Layout.fillWidth: true
//            height: 28
//            spacing: 2
//            layoutDirection: Qt.RightToLeft

//            Repeater {
//                model: accountManager.enabledAccounts

//                Image {
//                    anchors.verticalCenter: accountsRow.verticalCenter
//                    width: 24
//                    height: 24
//                    sourceSize.height: height
//                    sourceSize.width: width
//                    source: "image://statustheme/2/online"
//                }
//            }
//        }

        Button {
            Layout.fillWidth: true
            text: "Click me"
            onClicked: statusMenu.popup()
        }
    }
}
