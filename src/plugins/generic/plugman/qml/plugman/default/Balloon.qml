// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: border

    width: 150
    height: 150
    z: 100500
    radius: 35
    color: "transparent"
    smooth: true

    Rectangle {
        id: background

        anchors.fill: parent
        color: "black"
        radius: border.radius
        opacity: 0.6
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InOutQuad;
        }
    }
}
