import Qt 4.7

TextEdit {
	id: text
	property bool delivered: false
	property date time
	property string body: "body"
	width: parent.width
	readOnly: true
	selectByMouse: true
	color: "gray"
	text: time + ": " + body
	wrapMode: "WordWrap"
}
