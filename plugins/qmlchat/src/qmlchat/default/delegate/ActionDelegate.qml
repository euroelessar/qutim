import QtQuick 2.2

MessageText {
	id: text
	property bool incoming: true
	property bool delivered: false
	property QtObject unit

	width: parent.width
	color: incoming ? "#ff6600" : "#0078ff"
	text: "text"
}
