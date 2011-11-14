import QtQuick 1.0

import QtQuick 1.1
import com.nokia.meego 1.0
import "constants.js" as UI

Item {
    id: listItem

    signal clicked
    property alias pressed: mouseArea.pressed

    property int titleSize: UI.LIST_TILE_SIZE
    property int titleWeight: Font.Bold
    property color titleColor: theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED : UI.LIST_TITLE_COLOR

    property int subtitleSize: UI.LIST_SUBTILE_SIZE
    property int subtitleWeight: Font.Light
    property color subtitleColor: theme.inverted ? UI.LIST_SUBTITLE_COLOR_INVERTED : UI.LIST_SUBTITLE_COLOR
	
	property alias iconSource: image.source
	property alias title: mainText.text
	property alias subtitle: subText.text
	
//	property var __icons: model.icons

    height: UI.LIST_ITEM_HEIGHT
    width: parent.width

    BorderImage {
        id: background
        anchors.fill: parent
        // Fill page porders
        anchors.leftMargin: -UI.MARGIN_XLARGE
        anchors.rightMargin: -UI.MARGIN_XLARGE
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
    }

    Row {
        anchors.fill: parent
        spacing: UI.LIST_ITEM_SPACING

        Image {
			id: image
            anchors.verticalCenter: parent.verticalCenter
            visible: source != ""
            width: UI.LIST_ICON_SIZE
            height: UI.LIST_ICON_SIZE
            source: model.iconSource ? model.iconSource : ""
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                id: mainText
                text: model.title
                font.weight: listItem.titleWeight
                font.pixelSize: listItem.titleSize
                color: listItem.titleColor
            }

            Label {
                id: subText
                text: model.subtitle
                font.weight: listItem.subtitleWeight
                font.pixelSize: listItem.subtitleSize
                color: listItem.subtitleColor

                visible: text != ""
            }
        }

//		Repeater {
//            anchors.verticalCenter: parent.verticalCenter
//			model: __icons
//			Image {
//	            anchors.verticalCenter: parent.verticalCenter
//	            width: UI.LIST_ICON_SIZE
//	            height: UI.LIST_ICON_SIZE
//	            source: modelData
//	        }
//		}
    }
    MouseArea {
        id: mouseArea;
        anchors.fill: parent
        onClicked: {
            listItem.clicked();
        }
    }
}
