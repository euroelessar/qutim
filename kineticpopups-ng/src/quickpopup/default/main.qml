import QtQuick 1.0
import qutIM 0.3

Rectangle {
	id: main

	width: 300
	height: childrenRect.height + 15
	color: "transparent"

	PopupAttributes {
		frameStyle: PopupAttributes.ToolTip
	}

	BorderImage {
		anchors.fill: main
		border.left: 5; border.top: 5
		border.right: 5; border.bottom: 5
		source: "images/background.png"
		opacity: 0.8
	}

	Image {
		id: image
		source: "images/qutim.svg"
		width: 50
		fillMode: Image.PreserveAspectFit

		anchors {
			top: main.top
			left: main.left
			topMargin: 15
			leftMargin: 5
		}
	}

	Text {
		id: title
		text: "Title"

		anchors {
			top: main.top
			left: image.right
			right: main.right
			topMargin: 10
			leftMargin: 5
			rightMargin: 5
		}

		elide: Text.ElideMiddle
		color: "white"
		font.bold: true
		style: Text.Sunken
		styleColor: "black"
		font.pixelSize: text.font.pixelSize + 2
	}

	Text {
		id: text
		wrapMode: Text.Wrap

		anchors {
			top: title.bottom
			left: title.left
			right: title.right
			topMargin: -15
		}

		color: "white"
		style: Text.Sunken
		styleColor: "black"
	}

    MouseArea {
		anchors.fill: main
		acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
			if (mouse.button == Qt.RightButton)
				popup.ignore();
			else
				popup.accept();
        }
    }

	Connections {
		target: popup
		onNotifyAdded: {
			text.text = text.text + "<br />" + notify.text;
			title.text = notify.title;
		}
	}
}
