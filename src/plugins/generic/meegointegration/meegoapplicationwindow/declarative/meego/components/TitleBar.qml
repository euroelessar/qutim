// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: parent ? parent.width : 600
    height: 80

    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#e0e0e0";
        }
        GradientStop {
            position: 1.00;
            color: "#c0c1c4";
        }
    }

    Rectangle {
        width:  parent.width;
        height: 1;
        anchors.bottom: parent.bottom;
        color: "#868686"
    }
}
