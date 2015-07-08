import QtQuick 1.0
import "Constants.js" as UI

Item {
	id: root

	width: parent.width
	height: childrenRect.height
	Item {
		id: header
		height: childrenRect.height

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		Text {
			id: senderNameItem
			text: model.senderName
			color: model.incoming ? UI.incomingColor : UI.outgoingColor
			font.bold: true
			font.pointSize: UI.fontSize
			MouseArea {
				id: area
				anchors.fill: parent

				acceptedButtons: Qt.LeftButton
				onPressed: {
					console.log("pressed");
//					controller.appendText(message.sender + ":");
				}
			}
		}
		Text {
			anchors.right: parent.right
			id: messageDate
			font.pointSize: UI.fontSize
			text: UI.formatTime(model.time, true)
			color: UI.timeColor
		}
	}
	Message {
		id: messageBody

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.topMargin: 0
		anchors.top: header.bottom
	}
}
