import Qt 4.7

Rectangle {
	id: background
	width: 250
	height: 90
	color:"transparent"

	BorderImage {
		source: "images/background.png"
		border.left: 32; border.top: 32
		border.right: 32; border.bottom: 32
		opacity:0.9
		anchors.fill:parent
	}

	Text {
		id: title
		text: popupTitle
		color: "#FFF"
		font.pointSize: 11
		wrapMode: "WordWrap"
		font.bold: true

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			leftMargin: 20
			rightMargin: 15
			topMargin:15
		}
	}


	Item {
		id: body
		anchors {
			top: title.bottom
			left: parent.left
			right: parent.right
			bottom: parent.bottom
			topMargin: 10
		}
		Image {
			id: avatar
			source: (popupImage != "") ? popupImage : "images/buddyicon.png"
			width: 32
			height: 32
			fillMode: Image.PreserveAspectCrop
			anchors {
				top: parent.top
				left: parent.left
				leftMargin: 10
			}
		}
		Text {
			id: bodyText
			text: popupBody
			color: "#FFF"
			font.pointSize: 10
			wrapMode: "WrapAnywhere"
			anchors {
				left: avatar.right
				leftMargin: 10
				top: parent.top
				bottom: parent.bottom
				right:parent.right
				rightMargin: 7
			}
			onTextChanged: {
				show_timer.restart();
				background.height = (body.y + body.height + 15); //hack
			}
		}
	}

	MouseArea {
		anchors.fill: parent
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
		running:true
		interval: timeout
		onTriggered: {
			popupWidget.activated();
		}

	}
}
