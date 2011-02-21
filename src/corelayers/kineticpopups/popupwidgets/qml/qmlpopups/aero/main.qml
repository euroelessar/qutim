import Qt 4.7

Rectangle {
	id: background
	width: 250
	height: childrenRect.height + 5
	color:palatte.shadow
	gradient: Gradient {
		GradientStop { position: 0.0; color: Qt.lighter(palatte.highlight,1.65) }
		GradientStop { position: 1.0; color: palatte.base }
	}
	border.width: 1
	border.color: palatte.shadow

	SystemPalette { id: palatte; colorGroup: SystemPalette.Active }

	Text {
		id: header
		text:  popupTitle
		color: palatte.text
		font.bold: true
		style: Text.Outline; styleColor: palatte.light
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
		color: palatte.text
		font.pointSize: 10
		wrapMode: "WordWrap"
		text: popupBody
		style: Text.Outline; styleColor: palatte.light
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
