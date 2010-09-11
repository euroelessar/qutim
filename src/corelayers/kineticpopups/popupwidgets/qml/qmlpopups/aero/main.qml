import Qt 4.7

Rectangle {
	id: background
	width: 250
	height: 75
	color:"transparent"

	SystemPalette { id: palatte; colorGroup: SystemPalette.Active }

	Text {
		id: title
		text: popupTitle
		color: palatte.text
		font.pointSize: 10
		wrapMode: "WordWrap"
		font.bold: true
		style: Text.Outline; styleColor: palatte.light
		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			leftMargin: 15
			rightMargin: 15
			topMargin:5
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
			color: palatte.text
			font.pointSize: 10
			wrapMode: "WordWrap"
			anchors {
				left: avatar.right
				leftMargin: 5
				top: parent.top
				bottom: parent.bottom
				right:parent.right
				rightMargin: 7
			}
			onTextChanged: {
				show_timer.restart();
				background.height = (body.y + body.height + 15); //hack
			}
			style: Text.Outline; styleColor: palatte.light


//			effect: DropShadow {
//				color: "white"
//				blurRadius: 8
//				offset.x: 0
//				offset.y: 0
//			}
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
