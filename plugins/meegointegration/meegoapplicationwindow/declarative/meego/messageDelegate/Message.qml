import QtQuick 1.0
import "Constants.js" as UI

Item {
	id: messageBody
	width: parent.width
	height: childrenRect.height

	Image {
		anchors.left: parent.left
		anchors.top: parent.top
		id: statusItem
		source: model.delivered ? UI.receivedImage : UI.sendImage;
	}

	MessageText {
		id: textItem
		text: model.html

		anchors.top: parent.top
		anchors.right: parent.right
		anchors.left: statusItem.right
		anchors.leftMargin: 5
	}
}
