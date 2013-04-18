// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.1

Item {
    id: listItem
    height: 28
    width: parent.width
    clip: true
    signal clicked
    signal pressAndHold
    property bool hasAvatar: model.avatar !== ""

    property string fontFamily: "DejaVu Sans"
    
    property int titleSize: 10 //UI.LIST_TILE_SIZE
    property int titleWeight: Font.Bold
    property color titleColor: "#000000" //theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED : UI.LIST_TITLE_COLOR

    property int subtitleSize: 9 //UI.LIST_SUBTILE_SIZE
    property int subtitleWeight: Font.Light
    property color subtitleColor: "#111111" //theme.inverted ? UI.LIST_SUBTITLE_COLOR_INVERTED : UI.LIST_SUBTITLE_COLOR
    
/*    BorderImage {
        id: background
        anchors.fill: parent
        // Fill page porders
        anchors.leftMargin: -UI.MARGIN_XLARGE
        anchors.rightMargin: -UI.MARGIN_XLARGE
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
    } */
    
    
    Row {
        anchors.fill: parent
        spacing: 2
        
        Image {
            id: avatarImage
            anchors.verticalCenter: parent.verticalCenter
            width: 24 //UI.LIST_ICON_SIZE
            height: 24 //UI.LIST_ICON_SIZE
            source: model.avatar
//                    source: listItem.hasAvatar ? "image://theme/meegotouch-avatar-frame-small" : ""
   /*         Image {
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
            } */
        }
        
        Column {
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: mainText
                width: listItem.width - avatarImage.width - 4
                text: model.title
                font.weight: listItem.titleWeight
                font.pixelSize: listItem.titleSize
                color: listItem.titleColor
                font.family: listItem.fontFamily
                maximumLineCount: 1
                elide: Text.ElideRight
            }

            Text {
                id: subText
                width: listItem.width - avatarImage.width - 4
                text: model.statusText
                font.weight: listItem.subtitleWeight
                font.pixelSize: listItem.subtitleSize
                color: listItem.subtitleColor
                font.family: listItem.fontFamily
                maximumLineCount: 1
                elide: Text.ElideRight

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
