import QtQuick 1.0
import "Constants.js" as UI

Loader {
	id: root
	width: ListView.view.width
	sourceComponent: UI.guessComponent(model)
}

//Component {
//	id: container
//	//TODO separate this items and move to component.createObject
//	Item {
//		id: message
//		width: parent.width
//		x: -parent.width
//		property bool delivered: isDelivered
//		property variant delegate
//		height: childrenRect.height + 5
//		opacity: 1

//		Component.onCompleted: {
//			var formattedTime = Qt.formatDateTime(time, "dd.MM.yyyy (hh:mm:ss)");
//			if (action) {
//				var component = Qt.createComponent("ActionDelegate.qml");
//				delegate = component.createObject(message);
//				delegate.text =  "<b>" + sender + "</b>: " + body;
//				delegate.incoming = isIncoming;
//			} else if (service) {
//				var component = Qt.createComponent("ServiceDelegate.qml");
//				delegate = component.createObject(message);
//				delegate.body = body;
//				delegate.time = formattedTime;
//			} else if (append) {
//				var component = Qt.createComponent("Message.qml");
//				delegate = component.createObject(message);
//				delegate.body =  body;
//				delegate.delivered = message.delivered;
//			} else {
//				var component = Qt.createComponent("CommonMessageDelegate.qml");
//				delegate = component.createObject(message);
//				delegate.body =  body;
//				delegate.time = formattedTime;
//				delegate.sender = sender;
//				delegate.incoming = isIncoming;
//				delegate.delivered = message.delivered;
//			}

//			message.state = "show";
//		}

//		onDeliveredChanged: {
//			if (!delegate || delegate == "undefined" || delegate.delivered == "undefined")
//				return;
//			delegate.delivered = message.delivered;
//		}

//		states: State {
//			name: "show"
//			PropertyChanges { target: message; x: 0 }
//			PropertyChanges { target: message; opacity: 1 }
//		}

//		transitions: Transition {
//			//NumberAnimation { properties: "x"; duration: 400 }
//			//NumberAnimation { properties: "opacity"; duration: 200 }
//		}

//	}
//}
