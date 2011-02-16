import QtQuick 1.0

Text {
	property bool incoming: true
	id: text
	color: incoming ? "#ff6600" : "#0078ff"
	text: "text"
	wrapMode: "WordWrap"
}
