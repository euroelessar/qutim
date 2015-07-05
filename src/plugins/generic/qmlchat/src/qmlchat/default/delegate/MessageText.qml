import QtQuick 2.2

MouseArea {
	id: area

	property string text
	property color color

	width: parent.width
	height: childrenRect.height
	hoverEnabled: true;

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
}
