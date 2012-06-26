// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import "constants.js" as UI

Item {
    id: listItem
    height: UI.LIST_ITEM_HEIGHT
    width: parent.width
    clip: true
    signal clicked
    signal pressAndHold
    property bool hasAvatar: model.avatar != ""
    
    property int titleSize: UI.LIST_TILE_SIZE
    property int titleWeight: Font.Bold
    property color titleColor: theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED : UI.LIST_TITLE_COLOR

    property int subtitleSize: UI.LIST_SUBTILE_SIZE
    property int subtitleWeight: Font.Light
    property color subtitleColor: theme.inverted ? UI.LIST_SUBTITLE_COLOR_INVERTED : UI.LIST_SUBTITLE_COLOR
    
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
        
        Item {
            id: avatarImage
            anchors.verticalCenter: parent.verticalCenter
            width: UI.LIST_ICON_SIZE
            height: UI.LIST_ICON_SIZE
//                    source: listItem.hasAvatar ? "image://theme/meegotouch-avatar-frame-small" : ""
            Image {
                anchors.fill: parent
                MaskEffect {
                    id: maskEffect
                    mask: Image {
                        source: "image://theme/meegotouch-avatar-mask-small"
                    }
                }
                effect: listItem.hasAvatar ? maskEffect : null
                source: listItem.hasAvatar ? model.avatar : "image://theme/icon-m-content-avatar-placeholder"
            }
            Image {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: UI.LIST_ICON_SIZE / 2
                height: UI.LIST_ICON_SIZE / 2
                source: model.iconSource
            }
        }
        
        Column {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                id: mainText
                text: model.title
                font.weight: listItem.titleWeight
                font.pixelSize: listItem.titleSize
                color: listItem.titleColor
                maximumLineCount: 1
            }

            Label {
                id: subText
                text: model.subtitle
                font.weight: listItem.subtitleWeight
                font.pixelSize: listItem.subtitleSize
                color: listItem.subtitleColor
                maximumLineCount: 1

                visible: text != ""
            }
        }
    }
    MouseArea {
        id: mouseArea;
        anchors.fill: parent
        onClicked: {
            listItem.clicked();
        }
        onPressAndHold: {
            listItem.pressAndHold();
        }
    }
}
