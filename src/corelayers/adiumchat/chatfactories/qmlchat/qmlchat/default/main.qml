import QtQuick 1.0
import "delegate"

Rectangle {
	id: main
	width: 300
	height: 200

	ListModel {
		id: messageModel
//		ListElement {
//			sender: "Sauron";
//			isIncoming: false;
//			body: "Service message";
//			time : "2010.03.04";
//			isDelivered : false
//			service: true
//			action: false
//		}
//		ListElement {
//			sender: "Sauron";
//			isIncoming: true;
//			body: "У попа была собака, он её любил, она съела кусок мяса, он её убил";
//			time : "2010.03.04";
//			isDelivered : true
//			service: false
//			action: false
//		}
//		ListElement {
//			sender: "Sauron";
//			isIncoming: true;
//			body: "У попа была собака, он её любил, она съела кусок мяса, он её убил";
//			time : "2010.03.04";
//			isDelivered : true
//			service: false
//			action: false
//			append: true
//		}
//		ListElement {
//			sender: "SauronTheDark";
//			isIncoming: false;
//			body: "Text";
//			time : "Time";
//			isDelivered : false
//			service: false
//			action: true
//		}
//		ListElement {
//			sender: "Sauron";
//			isIncoming: false;
//			body: "Text";
//			time : "Time";
//			isDelivered : false
//			service: true
//		}
	}

	MessageDelegate {
		id: messageDelegate
	}

	ListView {
		id: messageView
		anchors.margins: 10
		model: messageModel
		anchors.fill: parent
		delegate: messageDelegate
	}

	Connections {
		target: controller
		onMessageAppended: {
			var myMessage = message;
			var index = messageModel.count-1;
			myMessage.append = false;
			//TODO add date format
			//myMessage.time = Qt.formatDateTime(message.time, "dd.yy.hh");
			if (index != -1) {
				var prevMessage = messageModel.get(index);
				if ((prevMessage.sender == message.sender) && !(prevMessage.action || prevMessage.service))
					myMessage.append = true;
			}
			messageModel.append(myMessage);
			messageView.incrementCurrentIndex();
		}
		onMessageDelivered: {
			for (var i = messageModel.count-1;i!=-1;i--) {
				if (messageModel.get(i).mid = mid) {
					messageModel.setProperty(i, "isDelivered", true);
					break;
				}
			}
		}
	}

}
