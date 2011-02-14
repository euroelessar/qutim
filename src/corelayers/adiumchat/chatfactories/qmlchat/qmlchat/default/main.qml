import Qt 4.7

Rectangle {
	id: main
	width: 50
	height: 40

	ListModel {
		id: messageModel
//		ListElement {sender: "Sauron"; isIncoming: false; body: ""; time : "Time"; isDelivered : false}
//		ListElement {sender: "Sauron"; isIncoming: true; body: "У попа была собака, он её любил, она съела кусок мяса, он её убил"; time : "Time"; isDelivered : true}
//		ListElement {sender: "Sauron"; isIncoming: false; body: "Text"; time : "Time"; isDelivered : false}
	}

	ListView {
		id: messageView
		anchors.margins: 10
		model: messageModel
		anchors.fill: parent
		delegate: Component {
			id: messageDelegate
			Item {
				id: test
				width: parent.width
				height: Math.max(messageText.height + messageText.pos.y, messageSender.height + messageSender.y)
				Item {
					id: messageHeader
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.top: parent.top
					anchors.topMargin: 10

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
						anchors.leftMargin: 5
						anchors.top: messageSender.bottom
						source: isDelivered ? "images/bullet-received.png" : "images/bullet-send.png"
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
			}
		}
	}

	Connections {
		target: controller
		onMessageAppended: {
			var index = messageModel.count-1;
			if ((index != -1) && (messageModel.get(index).sender == message.sender)) {
				var body = messageModel.get(index).body + "<br />" + message.body;
				messageModel.setProperty(index, "body", body);
			} else
				messageModel.append(message);
		}
		onMessageDelivered: {
			console.log ("mid = " + mid);
			for (var i = messageModel.count-1;i!=-1;i--) {
				console.log("search mid: " + messageModel.get(i).mid);
				if (messageModel.get(i).mid = mid) {
					messageModel.setProperty(i, "isDelivered", true);
					break;
				}
			}
		}
	}

}
