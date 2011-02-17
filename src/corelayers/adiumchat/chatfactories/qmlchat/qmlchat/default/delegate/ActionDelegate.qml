import QtQuick 1.0

TextEdit {
	id: text
	property bool incoming: true
	property bool delivered: false
	width: parent.width
	readOnly: true
	selectByMouse: true
	color: incoming ? "#ff6600" : "#0078ff"
	text: "text"
	wrapMode: "WordWrap"
}
