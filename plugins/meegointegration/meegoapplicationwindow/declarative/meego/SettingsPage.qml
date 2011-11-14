import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0

Page {
	id: root
	property QtObject model
	
	/*ListView {
		id: listView
		model: root.model
		anchors.fill: parent
		delegate: ItemDelegate {
			anchors.leftMargin: 5
			title: model.display
			subtitle: ""
			iconSource: ""
//			iconSource: model.decoration
			onClicked: {
			}
			MoreIndicator {
				anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 5 }
			}
		}
	}*/

	Component.onCompleted: console.log("SettingsPage", model)
}
