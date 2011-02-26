import Qt 4.7

MessageText {
	id: text
	property bool incoming: true
	property bool delivered: false
	width: parent.width
	color: incoming ? "#ff6600" : "#0078ff"
	text: "text"
}
