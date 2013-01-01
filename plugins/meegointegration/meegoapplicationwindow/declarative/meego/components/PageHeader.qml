import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
	id:header
	// Common public API
	property bool checked: false
	property bool checkable: false
	property bool clickable: false
	property alias pressed: mouseArea.pressed
	property alias text: label.text
	property url iconSource

	signal clicked

	property Style platformStyle: ButtonStyle {}

	implicitWidth: iconAndLabel.prefferedWidth
	implicitHeight: platformStyle.buttonHeight
	width: parent ? parent.width : 600
	height: screen.currentOrientation === Screen.Portrait ? 72 : 46
	color: "steelblue"
	z: 100500
	anchors.top: parent.top

	property alias font: label.font

	property int padding_xlarge: 16
	property int button_label_marging: 10
	property int size_icon_default: 32

	Item {
		id: iconAndLabel
		property real xMargins: icon.visible ? (padding_xlarge * (label.visible ? 3 : 2)) : (button_label_marging * 2)
		property real prefferedWidth: xMargins + (icon.visible ? icon.width : 0) + (label.visible ? label.prefferedSize.width : 0)

		width: xMargins + (icon.visible ? icon.width : 0) + (label.visible? label.width : 0)
		height: platformStyle.buttonHeight

		anchors.verticalCenter: header.verticalCenter
		anchors.left: header.left
		anchors.verticalCenterOffset: -1

		Image {
			id: icon
			source: header.iconSource
			x: padding_xlarge
			anchors.verticalCenter: iconAndLabel.verticalCenter
			width: size_icon_default
			height: size_icon_default
			visible: source != ""
		}

		Label {
			id: label
			x: icon.visible ? (icon.x + icon.width + padding_xlarge) : button_label_marging
			anchors.verticalCenter: iconAndLabel.verticalCenter
			anchors.verticalCenterOffset: 1

			property real availableWidth: header.width - (indicator.visible ? (indicator.width + padding_xlarge) : 0) - iconAndLabel.xMargins - (icon.visible ? icon.width : 0)
			width: Math.min(prefferedSize.width, availableWidth)

			font.family: header.platformStyle.fontFamily
			font.weight: header.platformStyle.fontWeight
			font.capitalization: header.platformStyle.fontCapitalization
			color: "white"
			text: qsTr("Header")
			font.pixelSize: 32
			font.bold: true
			z: 2
			elide: Text.ElideMiddle

			Label {
				id: prefferedSize
				font: parent.font
				text: parent.text
				visible: false
			}
			property alias prefferedSize: prefferedSize
		}

		Label {
			id: shadow
			property int offset: 2

			anchors.fill: label
			anchors.topMargin: offset
			anchors.leftMargin: offset
			anchors.bottomMargin: -offset
			anchors.rightMargin: -offset

			color: "black"
			text: label.text
			z: label.z - 1
			opacity: 0.7
			font.pixelSize: label.font.pixelSize
			font.bold: label.font.bold
			elide: label.elide
		}
	}

	Image {
		id: indicator
		anchors.right: parent.right
		width: size_icon_default
		height: size_icon_default
		anchors.rightMargin: padding_xlarge
		anchors.verticalCenter: iconAndLabel.verticalCenter
		source: "image://theme/meegotouch-combobox-indicator-inverted"
		visible: header.clickable
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent

		onClicked: {
			if (header.checkable)
				header.checked = !header.checked;
			header.clicked();
		}
	}

	Rectangle {
		width: parent.width
		height: 1
		anchors.bottom: parent.bottom
		color: "black"
	}
}
