import QtQuick 1.0
import "Constants.js" as UI

MouseArea {
	id: area

	property bool selectMode: false
	property string text
	property color color

	width: parent.width
	height: childrenRect.height
//	hoverEnabled: true;

//	onPressAndHold: {
//		selectMode = true;
//		console.log("HOLD!!");
//	}
//	onDoubleClicked: selectMode = true;
//	onExited: selectMode = false;

	Text {
		id: message

		width: area.width
		color: area.color
		textFormat: "RichText"
		text: area.text
		font.pointSize: UI.fontSize
		wrapMode: "WordWrap"
		onLinkActivated: Qt.openUrlExternally(link)
//		opacity:  area.selectMode ? 0 : 1
	}

//	TextEdit {
//		id: edit

//		anchors.fill: message

//		text: message.text
//		font: message.font
//		color: message.color
//		opacity:  area.selectMode ? 1 : 0
//		readOnly: true
//		selectByMouse: true
//		wrapMode: message.wrapMode
//	}

}
