import QtQuick 2.1

Rectangle {
    width: parent ? parent.width : 400
    height: 48
    z: 100500
    anchors.top: parent.top

    gradient: Gradient {
         GradientStop { position: 0.0; color: "#5878A1" }
         GradientStop { position: 1.0; color: "#466392" }
    }

    Rectangle {
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: "black"
    }
}
