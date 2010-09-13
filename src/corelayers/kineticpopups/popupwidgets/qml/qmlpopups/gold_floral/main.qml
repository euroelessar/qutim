/*
* QML popup style created by:
*	  R.S.A. aka .faust
*	  E-mail: rsa dot aka dot faust at gmail.com
*	  Jabber: rsa_aka_faust@jabber.ru
*	  v 1.0b
*/

import Qt 4.7

BorderImage {
	id: borderimage
	width: 250
	height: 120
 border.bottom: 0
 border.top: 0
 border.right: 1
 border.left: 1
	source: "images/background.png"

	Text {
		id: title
		text: popupTitle
		x: 10
		y: 10
		width: 191
		height: 20
		color: "#ffd700"
		wrapMode: "WordWrap"
		font.italic: true
		smooth: true
	}

	Text {
		id: bodyText
		text: popupBody
		x: 42
		y: 34
		width: 190
		height: 71
		color: "#ffffff"
		smooth: true
		wrapMode: "WordWrap"
	}

	Image {
		id: avatar
		x: 5
		y: 73
		width: 32
		height: 32
		source: (popupImage != "") ? popupImage: "images/buddyicon.png"
		fillMode: Image.PreserveAspectCrop
	}

	MouseArea {
		id: popup_mousearea
		x: 0
		y: 0
		width: 250
		height: 120
		acceptedButtons: Qt.LeftButton | Qt.RightButton
		onClicked: {
		if (mouse.button == Qt.LeftButton)
			popupWidget.accept();
		else
			popupWidget.ignore();
		}
	}

	Timer {
		id: show_timer
		running: true
		interval: timeout
		onTriggered: {
			popupWidget.activated();
		}
	}
}
