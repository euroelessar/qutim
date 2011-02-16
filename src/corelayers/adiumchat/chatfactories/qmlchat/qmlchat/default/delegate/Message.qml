import QtQuick 1.0

//TODO rewrite delivered

Text {
	width: parent.width

	property string body: "body"
	property bool delivered: false
	id: messageBody
	text: delivered ? " <img src=../images/bullet-received.png align=center> " + body : "<img src=../images/bullet-send.png align=center> " + body
	wrapMode: "WordWrap"

}
