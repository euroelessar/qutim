import QtQuick 2.1
import QtQuick.Controls 1.0
import "settings"

Item {
    ContactList {
        id: contactList
        onSettingsRequested: settingsDialog.show()
    }

    Component.onCompleted: {
        contactList.show();
    }

    SettingsDialog {
        id: settingsDialog

        SettingsItem {
            title: qsTr("Foo")
            iconName: "applications-internet"

            component: Item {
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
            }
        }

        SettingsItem {
            title: qsTr("Boo")
            iconName: "applications-system"

            component: Column {

                Label {
                    text: qsTr("Boo settings")
                }

                TextArea {
                    width: parent.width
                    text: "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor "+
                          "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor "+
                          "incididunt ut labore et dolore magna aliqua.\n Ut enim ad minim veniam, quis nostrud "+
                          "exercitation ullamco laboris nisi ut aliquip ex ea commodo cosnsequat. ";
                }
            }
        }
    }

    SystemPalette {
        id: systemPalette
    }

}
