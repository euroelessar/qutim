import QtQuick 1.0
import qutIM 0.3

Rectangle {
	id: main

	width: 250
	height: childrenRect.height + 10
	color: "transparent"

	PopupAttributes {
		frameStyle: PopupAttributes.ToolTip
	}

	BorderImage {
		anchors.fill: main
		border.left: 5; border.top: 5
		border.right: 5; border.bottom: 5
		source: "images/background.png"
	}

	Text {
		id: title

		anchors {
			top: main.top
			left: main.left
			right: main.right
			topMargin: 5
			leftMargin: 5
			rightMargin: 5
		}

		wrapMode: Text.WrapAnywhere
		color: "white"
		font.bold: true
	}

	Text {
		id: text
		wrapMode: Text.WrapAnywhere

		anchors {
			top: title.bottom
			left: title.left
			right: title.right
			topMargin: 5
		}

		color: "white"
	}

    MouseArea {
		anchors.fill: main
        onClicked: {
			console.log("Test Enum: " + Notification.IncomingMessage);
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
