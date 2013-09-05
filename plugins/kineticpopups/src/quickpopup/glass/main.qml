import QtQuick 1.0
import qutIM 0.3

Rectangle {
	id: main

	width: 300
    height: childrenRect.height
	color: "transparent"

	PopupAttributes {
		id: attributes
		property int trimLength: 80

		frameStyle: PopupAttributes.AeroBlur
	}

	Image {
		id: image
		source: "images/qutim.svg"
		width: 30
		fillMode: Image.PreserveAspectFit

		anchors {
			top: main.top
			left: main.left
			topMargin: 15
			leftMargin: 5
		}
	}

	Text {
		id: subject
		//text: "Title"

		anchors {
			top: main.top
			left: image.right
			right: main.right
			topMargin: 5
			leftMargin: 5
			rightMargin: 5
		}

		elide: Text.ElideMiddle
		color: "black"
		font.bold: true
		style: Text.Outline
		styleColor: "white"
		font.pixelSize: body.font.pixelSize + 2
	}

	Text {
		id: body
		wrapMode: Text.Wrap
		//text: "Body Вот тестирую текст, много разного текста, он должен нормально отображаться"

		anchors {
			top: subject.bottom
			left: subject.left
			right: subject.right
			//topMargin: -15
		}

		color: "black"
		style: Text.Outline
		styleColor: "white"
	}

	ListView {
		id: actions

		z: 10
		height: 30
		orientation: ListView.Horizontal
		spacing: 10

		anchors {
			top: body.bottom
			left: body.left
			right: body.right
		}
		delegate: Text {
			id: actionDelegate
			text: model.modelData.text
			height: 15

			color: "black"
			style: Text.Outline
			styleColor: "white"
			font.italic: true
			font.underline: true

			MouseArea {
				id: actionArea
				anchors.fill: parent

				hoverEnabled: true
				acceptedButtons: Qt.LeftButton

				onClicked: {
					console.log("Action: " + model.modelData);
					model.modelData.trigger();
					mouse.accepted = true;
				}
			}

			states: [
				State {
					name: "hovered"
					when: actionArea.containsMouse
					PropertyChanges {
						target: actionDelegate
						font.underline: false
					}
				},
				State {
					name: "leaved"
					when: !actionArea.containsMouse
					PropertyChanges {
						target: actionDelegate
						font.underline: true
					}
				},
				State {
					name: "pressed"
					when: actionArea.pressed
					PropertyChanges {
						target: actionDelegate
						font.underline: false
						font.bold: true
					}
				}
			]
		}
	}

	MouseArea {
		id: acceptIgnoreArea
		anchors.fill: main
		acceptedButtons: Qt.LeftButton | Qt.RightButton
		onClicked: {
			console.log("Clicked");
			if (mouse.button == Qt.RightButton)
				popup.ignore();
			else
				popup.accept();
		}
	}

	Connections {
		target: popup
		onNotifyAdded: {
			var str = body.text;
			if (str.length && notify.text.length && str.length < attributes.trimLength)
				str = str + "<br /> ";
			str = str + notify.text;
			//trim
			if (str.length > attributes.trimLength)
				str = str.substring(0, attributes.trimLength - 3) + "...";
			body.text = str;

			subject.text = notify.title;
			actions.model = notify.actions;

            if(notify.avatar)
                image.source = notify.avatar;
		}
	}
}
