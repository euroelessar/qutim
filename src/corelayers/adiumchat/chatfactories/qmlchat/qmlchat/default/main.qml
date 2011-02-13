import Qt 4.7

Rectangle {
	id: main
	width: 600
	height: 400

	ListModel {
		id: messageModel
	}

	ListView {
		id: messageView
		model: messageModel
		anchors.fill: parent
		delegate: Rectangle {
			width: parent.width
			height: messageBody.height + 10
			id: messageDelegate
			Text {
				id: messageBody
				text: "<b>" + name + "</b> " + body;
				wrapMode: "Wrap"
			}
		}
	}

	Connections {
		target: controller
		onMessageAppended: {
			console.log("New message: " + message.body + " " + message.chatUnit.title + " " + message.account.name);

			var sender;
			console.log("SenderName: " + message.senderName);
			console.log("isIncoming: " + message.isIncoming);
			console.log("unit: " + message.chatUnit);
			console.log("unit.title: " + message.chatUnit.title);
			console.log("unit.account.name: " + message.account.name);
			if (message.senderName != "undefined")
				sender = message.senderName;
			else if (message.isIncoming)
				sender = message.account.name;
			else
				sender = message.chatUnit.title;

			messageModel.append(
				{"name" : sender,
				"subject" : message.subject,
				"body" : message.body
				}
			);
		}
	}
}
