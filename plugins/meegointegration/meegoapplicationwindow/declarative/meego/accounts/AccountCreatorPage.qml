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
    property variant contactList: serviceManager.contactList
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
			onClicked: {
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
}
