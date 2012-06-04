// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
        function save() {
        }
        function load() {
        }
    }
    property QtObject contactList: serviceManager.contactList
    property QtObject settingsLayer: serviceManager.settingsLayer
    ServiceManager {
		id: serviceManager
	}
    Component {
        id: protocolListComponent
        ProtocolListPage {
        }
    }

    tools: SettingsToolBarLayout {
        id: toolBarLayout
        page: root

        ToolIcon {
            visible: true
            platformIconId: "toolbar-add"
            onClicked: pageStack.push(protocolListComponent)
        }
    }
    ListView {
        anchors.fill: parent
        model: contactList.accounts
        delegate: ItemDelegate {
            title: modelData.name === "" ? modelData.id : modelData.name
            subtitle: modelData.id
            onClicked: settingsLayer.show(modelData)
            onPressAndHold: {
                menu.account = modelData
                menu.open()
            }
            MoreIndicator {
				anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 5
                }
			}
		}
    }
    Menu {
        id: menu
        property QtObject account
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                text: qsTr("Remove account")
                onClicked: queryDialog.open()
            }
        }
    }
    QueryDialog {
        id: queryDialog
        titleText: menu.account.id
        message: qsTr("Do you really want to remove account?")
        acceptButtonText: qsTr("Remove")
        rejectButtonText: qsTr("Cancel")
        onAccepted: menu.account.protocol.removeAccount(menu.account)
    }
}
