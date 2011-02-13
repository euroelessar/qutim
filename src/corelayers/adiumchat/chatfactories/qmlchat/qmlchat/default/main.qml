import Qt 4.7

Rectangle {
	id: main

	Component {
		id: messageDelegate
		Item {
			width: 600
			height: 400
			Text {
				text: text
			}
		}
	}

	ListModel {
		id: messageModel
		//property variable lastId: 0
	}

	ListView {
		id: messageView
		model: messageModel
		anchors.fill: parent
		delegate: messageDelegate
	}

	Connections {
		target: controller
		onMessageAppended: {
			console.log("Message: " + message.text + " " + message.unit);
			messageModel.append(message);
		}
	}
}
