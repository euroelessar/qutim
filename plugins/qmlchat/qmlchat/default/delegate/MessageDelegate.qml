import Qt 4.7

Component {
	id: container
	//TODO separate this items and move to component.createObject
	Item {
		id: message

        property bool delivered: isDelivered
		property variant delegate
		height: childrenRect.height + 5
		opacity: 1
        width: parent.width

		Component.onCompleted: {
			var formattedTime = Qt.formatDateTime(time, "dd.MM.yyyy (hh:mm:ss)");
			if (action) {
				var component = Qt.createComponent("ActionDelegate.qml");
				delegate = component.createObject(message);
				delegate.text =  "<b>" + sender + "</b>: " + body;
				delegate.incoming = isIncoming;
			} else if (service) {
                component = Qt.createComponent("ServiceDelegate.qml");
				delegate = component.createObject(message);
				delegate.body = body;
				delegate.time = formattedTime;
			} else if (append) {
                component = Qt.createComponent("Message.qml");
				delegate = component.createObject(message);
				delegate.body =  body;
				delegate.delivered = message.delivered;
			} else {
                component = Qt.createComponent("CommonMessageDelegate.qml");
				delegate = component.createObject(message);
				delegate.body =  body;
				delegate.time = formattedTime;
				delegate.sender = sender;
				delegate.incoming = isIncoming;
				delegate.delivered = message.delivered;
			}
		}

		onDeliveredChanged: {
            if (!delegate || delegate === "undefined" || delegate.delivered === "undefined")
				return;
			delegate.delivered = message.delivered;
		}
	}
}
