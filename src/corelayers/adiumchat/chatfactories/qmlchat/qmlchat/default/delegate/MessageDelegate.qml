import Qt 4.7

Component {
	id: container
	//TODO separate this items and move to component.createObject
	Item {
		id: message
		width: parent.width
		property bool delivered: isDelivered
		property variant delegate
		height: childrenRect.height + 5

		Component.onCompleted: {
			if (action) {
				var component = Qt.createComponent("ActionDelegate.qml");
				delegate = component.createObject(message);
				delegate.text =  "<b>" + sender + "</b>: " + body;
				delegate.incoming = isIncoming;
			} else if (service) {
				var component = Qt.createComponent("ServiceDelegate.qml");
				delegate = component.createObject(message);
				delegate.body = body;
				delegate.time = time;
			} else if (append) {
				var component = Qt.createComponent("Message.qml");
				delegate = component.createObject(message);
				delegate.body =  body;
				delegate.delivered = message.delivered;
			} else {
				var component = Qt.createComponent("CommonMessageDelegate.qml");
				delegate = component.createObject(message);
				delegate.body =  body;
				delegate.time = time;
				delegate.sender = sender;
				delegate.incoming = isIncoming;
				delegate.delivered = message.delivered;
			}
		}

		onDeliveredChanged: {
			if (delegate == "undefined" || delegate.delivered == "undefined")
				return;
			delegate.delivered = message.delivered;
		}

	}
}
