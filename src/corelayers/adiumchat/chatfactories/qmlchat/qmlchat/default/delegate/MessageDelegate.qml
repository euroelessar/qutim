import QtQuick 1.0

Component {
	id: container
	//TODO separate this items and move to component.createObject
	Item {
		id: commonMessage
		width: parent.width
		height: Math.max(messageText.height + messageText.y, messageSender.height + messageSender.y)
		Item {
			id: messageHeader
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.topMargin: 15

			Text {
				id: messageSender
				color: isIncoming ? "#ff6600" : "#0078ff"
				text: sender
				font.bold: true
				anchors.top: parent.top
				anchors.left: parent.left
			}

			Text {
				id: messageTime
				color: "#808080"
				text: time
				font.italic: true
				anchors.right: parent.right
				anchors.top: parent.top
			}

			Image {
				id: messageStatus
				anchors.left: parent.left
				anchors.top: messageSender.bottom
				source: isDelivered ? "../images/bullet-received.png" : "../images/bullet-send.png"
			}
			Text {
				anchors.left: messageStatus.right
				anchors.leftMargin: 5
				//anchors.bottom: parent.bottom
				anchors.top: messageStatus.top
				anchors.right: parent.right
				id: messageText
				text: body
				wrapMode: "WordWrap"
			}
		}

		Component.onCompleted: {
			if (action) {
				messageSender.text = sender + " " + body;
				messageText.visible = false;
				messageStatus.visible = false;
				commonMessage.height = messageHeader.anchors.topMargin + messageHeader.height + messageHeader.y;
			} else if (service) {
				messageSender.text = body;
				messageSender.color = "gray";
				messageSender.font.bold = false;
				messageText.visible = false;
				messageStatus.visible = false;
			} else if (append) {
				messageSender.visible = false;
				messageTime.visible = false;
				messageStatus.anchors.top = messageHeader.top
				//commonMessage.height = messageText.height
			}
		}
	}
}
