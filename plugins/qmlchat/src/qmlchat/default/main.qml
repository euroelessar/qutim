import QtQuick 2.2
import org.qutim 0.4
import "delegate"

Rectangle {
	id: main

    width: 300
    height: 200
    
    property QtObject controller
    property QtObject session: controller !== null ? controller.session : null
    property QtObject unit: session !== null ? session.unit : null

	ListModel {
		id: messageModel


        //ListElement {
        //    sender: "Sauron";
        //    isIncoming: false;
        //    body: "Service message";
        //    time : "2010.03.04";
        //    isDelivered : false
        //    service: true
        //    action: false
        //}
        //ListElement {
        //    sender: "Sauron";
        //    isIncoming: true;
        //    body: "<a href=\"http://qutim.org\">Link me</a>";
        //    time : "2010.03.04";
        //    isDelivered : true
        //    service: false
        //    action: false
        //}
        //ListElement {
        //    sender: "Sauron";
        //    isIncoming: true;
        //    body: "У попа была собака, он её любил, она съела кусок мяса, он её убил";
        //    time : "2010.03.04";
        //    isDelivered : true
        //    service: false
        //    action: false
        //    append: true
        //}
        //ListElement {
        //    sender: "SauronTheDark";
        //    isIncoming: false;
        //    body: "Text";
        //    time : "2010.03.04";
        //    isDelivered : false
        //    service: false
        //    action: true
        //}
        //ListElement {
        //    sender: "Sauron";
        //    isIncoming: false;
        //    body: "Text";
        //    time : "2010.03.04";
        //    isDelivered : false
        //    service: true
        //}
	}

	MessageDelegate {
		id: messageDelegate
	}

	Component {
		id: highlight
		Rectangle {
			width: parent.width
			height: messageView.currentItem.height
			color: "gray"
			opacity: 0.1
			y: messageView.currentItem.y
			Behavior on y {
				SpringAnimation {
					spring: 3
					damping: 0.2
				}
			}
		}
	}

	ListView {
		id: messageView
		anchors.margins: 10
		model: messageModel

		anchors.fill: parent

		delegate: messageDelegate

		highlight: highlight
		highlightFollowsCurrentItem: true
		focus: true

		// Only show the scrollbars when the view is moving.
		states: State {
			name: "ShowBars"
			when: messageView.movingVertically
			PropertyChanges { target: verticalScrollBar; opacity: 1 }
		}

		transitions: Transition {
			NumberAnimation { properties: "opacity"; duration: 400 }
		}

		Text {
			id: chatStateText

			color: "gray"
			opacity: 1
            text: {
                if (main.unit) {
                    switch (unit.chatState) {
                    case ChatUnit.ChatStateActive:
                        return qsTr("is active");
                    case ChatUnit.ChatStateInActive:
                        return qsTr("is inactive");
                    case ChatUnit.ChatStateGone:
                        return qsTr("is gone");
                    case ChatUnit.ChatStateComposing:
                        return qsTr("is composing");
                    case ChatUnit.ChatStatePaused:
                        return qsTr("is paused");
                    }
                }
                return "";
            }

			anchors.bottom: messageView.bottom
			anchors.left: messageView.left
			anchors.right: messageView.right

			horizontalAlignment: Text.AlignHCenter

			Behavior on opacity {
				NumberAnimation { properties: "opacity"; duration: 500 }
			}

		}
	}

	ScrollBar {
		id: verticalScrollBar
		width: 12; height:  messageView.height-12
		anchors.right:  messageView.right
		opacity: 0
		orientation: Qt.Vertical
		position: messageView.visibleArea.yPosition
		pageSize: messageView.visibleArea.heightRatio
	}

	Connections {
		target: controller
		onMessageAppended: {				
			//console.log ("Session :" + controller.session);
			//console.log("Unit :" + controller.unit);
			//console.log("ChatState :" + controller.unit.chatState)

			var myMessage = message;
			var index = messageModel.count-1;
			myMessage.append = false;
			myMessage.body = controller.parseEmoticons(message.body);
			console.log("Unit:" + message.unit)

            if (index !== -1) {
				var prevMessage = messageModel.get(index);
                if ((prevMessage.sender === message.sender) && !(prevMessage.action || prevMessage.service))
					myMessage.append = true;
			}
			messageModel.append(myMessage);
            var needSlide = controller.session.active && messageView.currentIndex === messageView.count -2;
			if (!myMessage.isIncoming || needSlide)
				messageView.currentIndex = messageView.count - 1;
		}
		onMessageDelivered: {
            for (var i = messageModel.count-1;i !== -1;i--) {
                if (messageModel.get(i).messageId === messageId) {
					messageModel.setProperty(i, "delivered", true);
					break;
				}
			}
		}
	}

	Connections {
		target:  session
		onActivated: {
			console.log("Activated : " + active);
		}
	}
}
