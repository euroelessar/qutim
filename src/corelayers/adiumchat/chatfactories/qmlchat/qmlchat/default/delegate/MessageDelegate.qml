import QtQuick 1.0

Component {
	id: container
	//TODO separate this items and move to component.createObject
	Item {
		id: message
		width: parent.width
		property bool delivered: isDelivered
		height: childrenRect.height + 5
		//height: Math.max(messageText.height + messageText.y, messageSender.height + messageSender.y)

		Component.onCompleted: {
			if (action) {
				var component = Qt.createComponent("ActionDelegate.qml");
				var delegate = component.createObject(message);
				delegate.text =  "<b>" + sender + "</b>: " + body;
				delegate.incoming = isIncoming;
			} else if (service) {
				var component = Qt.createComponent("ServiceDelegate.qml");
				var delegate = component.createObject(message);
				delegate.text =  "(" + time + "): " + body;
			} else if (append) {
				var component = Qt.createComponent("Message.qml");
				var delegate = component.createObject(message);
				delegate.body =  body;
				delegate.delivered = message.delivered;
			} else {
				var component = Qt.createComponent("CommonMessageDelegate.qml");
				var delegate = component.createObject(message);
				delegate.body =  body;
				delegate.time = time;
				delegate.sender = sender;
				delegate.incoming = isIncoming;
				delegate.delivered = message.delivered;
			}
		}
	}
}
