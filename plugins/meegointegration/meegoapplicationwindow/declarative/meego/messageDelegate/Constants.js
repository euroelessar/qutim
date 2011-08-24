.pragma library

var incomingColor = "#ff6600"
var outgoingColor = "#0078ff"
var serviceColor = "gray"
var timeColor = "gray"
var fontSize = 16

var sendImage = "images/bullet-send.png"
var receivedImage = "images/bullet-received.png"

var actionComponent = Qt.createComponent("ActionDelegate.qml");
var serviceComponent = Qt.createComponent("ServiceDelegate.qml");
var nextMessageComponent = Qt.createComponent("Message.qml");
var messageComponent = Qt.createComponent("CommonMessageDelegate.qml");

var fullTimeFormat = "(dd.MM.yyyy hh:mm:ss)"
var shortTimeFormat = "(hh:mm:ss)"

function guessComponent(model) {
	if (model.action)
		return actionComponent;
	if (model.service)
		return serviceComponent;
	if (model.appending)
		return nextMessageComponent;
	return messageComponent;
}

function formatTime(time, full) {
	return Qt.formatDateTime(time, full ? fullTimeFormat : shortTimeFormat)
}

// Equal to Qt::escape
function htmlEscape(text) {
	var rich = "";
	for (var i = 0; i < text.length; ++i) {
		if (text[i] == '<')
			rich += "&lt;"
		else if (text[i] == '>')
			rich += "&gt;"
		else if (text[i] == '&')
			rich += "&amp;"
		else if (text[i] == '""')
			rich += "&quot;"
		else
			rich += text[i]
	}
	return rich
}
