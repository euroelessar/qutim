import Qt 4.6

BorderImage {
	id: background
	source: "images/background.png"
	width: 250
	border.left: 35
	border.top: 35
	border.bottom: 35
	border.right: 35

	Text {
		id: title
		text: popupTitle
		color: "white"
		font.pointSize: 10
		wrap: true
		font.bold: true
		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			topMargin: 15
			leftMargin: 15
			rightMargin: 15
		}
	}

	Rectangle {
		id: separator
		height: 1
		color: "#595959"
		anchors {
			left: parent.left
			right: parent.right
			top: title.bottom
			topMargin: 5
			leftMargin: 7
			rightMargin: 7

		}
	}

	Item {
		id: body
		anchors {
			top: separator.bottom
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}
		Image {
			id: avatar
			source: (popupImage != "") ? popupImage : "images/buddyicon.png"
			width: 32
			height: 32
			fillMode: Image.PreserveAspectCrop
			anchors {
				verticalCenter: parent.verticalCenter
				left: parent.left
				leftMargin: 10
			}
		}
		Text {
			id: bodyText
			text: popupBody
			color: "#DDD"
			font.pointSize: 10
			wrap: true
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
		}

		MouseRegion {
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
