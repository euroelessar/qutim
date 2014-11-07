import QtQuick 2.0

Rectangle {
    id: item

    height: avatar.height
    anchors.margins: 2
    color: "#00000000"

    property alias backgroundColor: item.color
    property alias titleColor: titleText.color
    property alias subtitleColor: subtitleText.color

    Image {
        id: avatar
        width: 22
        height: 22
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            margins: 4
        }
        source: iconSource ? iconSource + '&size=24' : ''
    }

    Text {
        id: titleText
        anchors {
            left: avatar.right
            right: parent.right
            top: parent.top
            leftMargin: 4
        }
        text: title
        elide: Text.ElideRight
        font.pointSize: 12
    }

    Text {
        id: subtitleText
        anchors {
            left: avatar.right
            right: parent.right
            top: titleText.bottom
            bottom: parent.bottom
            leftMargin: 4
        }
        text: subtitle
        elide: Text.ElideRight
        font.pointSize: 8
    }
}
