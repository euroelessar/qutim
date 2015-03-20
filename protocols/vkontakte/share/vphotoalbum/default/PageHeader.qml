import QtQuick 2.1

PageHeaderBar {
    id: root
    property alias text: label.text

    state: "common"

    Text {
        id: label

        anchors.verticalCenter: root.verticalCenter
        anchors.horizontalCenter: root.horizontalCenter

        color: "white"
        text: qsTr("Header")
        font.bold: true
        font.pointSize: 12
        z: 2
        elide: Text.ElideMiddle
    }

    Text {
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
