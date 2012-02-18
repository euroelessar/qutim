import QtQuick 1.0
import "Constants.js" as UI

MessageText {
	id: text
	width: parent.width
	color: UI.serviceColor
	text: UI.formatTime(model.time, true) + ": " + model.html
}
