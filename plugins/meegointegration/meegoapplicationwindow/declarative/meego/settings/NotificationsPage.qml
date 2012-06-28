// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."
import "../constants.js" as UI

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
        delegate: Item {
            id: listItem
            width: parent.width
            height: UI.LIST_ITEM_HEIGHT
            
            property int titleSize: UI.LIST_TILE_SIZE
            property int titleWeight: Font.Bold
            property color titleColor: theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED : UI.LIST_TITLE_COLOR
            
            Label {
                id: mainText
                anchors.verticalCenter: parent.verticalCenter
                text: notifications.backendDescription(modelData)
                font.weight: listItem.titleWeight
                font.pixelSize: listItem.titleSize
                color: listItem.titleColor
                width: parent.width - moreIndicator.width - 10
            }
			MoreIndicator {
                id: moreIndicator
                anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 5 }
			}
            MouseArea {
                id: mouseArea;
                anchors.fill: parent
                onClicked: {
                    var backendType = modelData;
                    root.pageStack.push(backendPageComponent, { "backendType": backendType });
                    root.pageStack.find(function (page) { return true; }).load();
                }
            }
		}
	}
}
