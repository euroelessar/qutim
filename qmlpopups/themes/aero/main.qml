import Qt 4.6

Rectangle {
	id: background
	width: 250
	height: 60
	color: "transparent"

	Text {
		id: title
		text: popupTitle
		color: "black"
		font.pointSize: 10
		wrap: true
		font.bold: true
		style: Text.Outline; styleColor: "#CCC"
		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			topMargin: 5
			leftMargin: 15
			rightMargin: 15
		}

		effect: DropShadow {
			color: "white"
			blurRadius: 8
			offset.x: 0
			offset.y: 0
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
				verticalCenter: parent.verticalCenter
				left: parent.left
				leftMargin: 10
			}
		}
		Text {
			id: bodyText
			text: popupBody
			color: "black"
			font.pointSize: 10
			wrap: true
			anchors {
				left: avatar.right
				leftMargin: 5
				top: parent.top
				right:parent.right
				rightMargin: 7
			}
			onTextChanged: {
				show_timer.restart();
				var h = (body.height > avatar.height) ? body.height : avatar.height; //hack, why doesn't work Math.max()?
				background.height = (body.y + h + 10); //hack
			}

			effect: DropShadow {
				color: "white"
				blurRadius: 8
				offset.x: 0
				offset.y: 0
			}
			style: Text.Outline; styleColor: "#CCC"
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
