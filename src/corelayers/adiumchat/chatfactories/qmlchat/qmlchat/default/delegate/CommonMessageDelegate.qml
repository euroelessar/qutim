import Qt 4.7

Item {
	id: message
	property string sender: "Sauron"
	property date time: "2010.02.02"
	property bool incoming: true
	property string body: "Message text"
	property bool delivered: false

	width: parent.width
	height: childrenRect.height
	Item {
		id: header
		height: childrenRect.height

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		Text {
			id: senderName
			text: message.sender
			color: message.incoming ? "#ff6600" : "#0078ff"
			font.bold: true
		}
		Text {
			anchors.right: parent.right
			id: messageDate
			text: message.time
			color: "gray"
		}
	}
	Message {
		id: messageBody

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.topMargin: 0
		anchors.top: header.bottom

		body: message.body
		delivered: message.delivered
	}
}
