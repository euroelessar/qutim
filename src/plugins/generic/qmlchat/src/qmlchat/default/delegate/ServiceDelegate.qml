import QtQuick 2.2

MessageText {
	id: text
	property bool delivered: false
	property string time
	property string body: "body"
	property string sender: ""
	property QtObject unit

	width: parent.width
	color: "gray"
	text: time + ": " + body
}
