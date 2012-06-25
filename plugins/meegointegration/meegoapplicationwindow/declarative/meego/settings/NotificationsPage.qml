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
    Notifications {
        id: notifications
    }
    Component {
		id: backendPageComponent
		NotificationsBackendPage {
		}
	}
    ListView {
		id: listView
        model: notifications.backendTypes()
		anchors.fill: parent
		anchors.leftMargin: 20
		delegate: ItemDelegate {			
			title: notifications.backendDescription(modelData)
			subtitle: ""
			iconSource: ""
			onClicked: {
                var backendType = modelData;
				root.pageStack.push(backendPageComponent, { "backendType": backendType });
                root.pageStack.find(function (page) { return true; }).load();
			}
			MoreIndicator {
				anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 5 }
			}
		}
	}
}
