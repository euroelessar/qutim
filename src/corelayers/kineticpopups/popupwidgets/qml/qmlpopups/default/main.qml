import Qt 4.7

Item {
	id: background
	width: 250
	height: childrenRect.height + 5

	BorderImage {
		id: image
		source: "images/background.png"
		border.left: 32; border.top: 32
		border.right: 32; border.bottom: 32
		opacity:0.8
		anchors.fill: parent
	}

	Text {
		id: header
		text:  popupTitle
		color: "white"
		font.bold: true
		font.pointSize: 10

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			topMargin: 10
			leftMargin: 15
			rightMargin: 15
		}
	}

	Image {
		id: avatar
		source: (popupImage != "") ? popupImage : "images/buddyicon.png"
		width: 32
		height: 32
		fillMode: Image.PreserveAspectCrop
		anchors {
			top: header.bottom
			left: parent.left
			topMargin: 15
			leftMargin: 15
			bottomMargin: 10
		}
	}

	Text {
		id: body
		color: "white"
		font.pointSize: 10
		wrapMode: "WordWrap"
		text: popupBody
		anchors {
			top: avatar.top
			left: avatar.right
			right: parent.right
			leftMargin: 5
			rightMargin: 15
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
