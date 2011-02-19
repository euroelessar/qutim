import Qt 4.7

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

	TextEdit {
		id: text
		textFormat: "RichText"
		readOnly: true
		selectByMouse: true
		text: body
		wrapMode: "WordWrap"

		anchors.top: parent.top
		anchors.right: parent.right
		anchors.left: status.right
		anchors.leftMargin: 5
	}
}
