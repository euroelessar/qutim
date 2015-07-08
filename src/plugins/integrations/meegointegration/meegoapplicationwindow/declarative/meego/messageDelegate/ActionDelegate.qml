import QtQuick 1.0
import "Constants.js" as UI

MessageText {
	id: root
	width: parent.width
	color: incoming ? UI.incomingColor : UI.outgoingColor
	text: "<b>" + UI.htmlEscape(sender) + "</b>: " + model.html
}
