import QtQuick 1.0

Text {
	property bool incoming: true
	property bool delivered: false
	id: text
	color: incoming ? "#ff6600" : "#0078ff"
	text: "text"
	wrapMode: "WordWrap"
}
