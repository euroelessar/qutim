import QtQuick 2.2

Item {
	id: messageBody
	property string body: "body"
	property bool delivered: false
	width: parent.width
	height: childrenRect.height

	Image {
		anchors.left: parent.left
		anchors.top: parent.top
		id: status
		source: delivered ? "images/bullet-received.png" : "images/bullet-send.png"
	}

	MessageText {
		id: text
		text: body

		anchors.top: parent.top
		anchors.right: parent.right
		anchors.left: status.right
		anchors.leftMargin: 5
	}
}
