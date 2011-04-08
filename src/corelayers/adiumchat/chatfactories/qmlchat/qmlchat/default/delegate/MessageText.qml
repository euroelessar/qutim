import Qt 4.7

//Text {
//	id: message
//	textFormat: "RichText"
//	wrapMode: "WordWrap"
//	onLinkActivated: Qt.openUrlExternally(link)

////	MouseArea {
////		id: area
////		property bool selectMode: false
////		anchors.fill: parent
////		hoverEnabled: true
////		//acceptedButtons: Qt.NoButton
////		onPressAndHold: console.log("Hold!!!");
////	}

////	TextEdit {
////		id: edit
////		text: message.text
////		anchors.fill: message
////		font: message.font
////		opacity:  0
////		readOnly: true
////		selectByMouse: true
////	}
//}

MouseArea {
	id: area

	property bool selectMode: false
	property string text
	property color color

	width: parent.width
	height: childrenRect.height
	hoverEnabled: true;

	onPressAndHold: {
		selectMode = true;
		console.log("HOLD!!");
	}
	onDoubleClicked: selectMode = true;
	onExited: selectMode = false;

	Text {
		id: message

		width: area.width

		color: area.color
		textFormat: "RichText"
		text: area.text
		wrapMode: "WordWrap"
		onLinkActivated: Qt.openUrlExternally(link)
		opacity:  area.selectMode ? 0 : 1
	}

	TextEdit {
		id: edit

		anchors.fill: message

		text: message.text
		font: message.font
		color: message.color
		opacity:  area.selectMode ? 1 : 0
		readOnly: true
		selectByMouse: true
		wrapMode: message.wrapMode
	}

}
