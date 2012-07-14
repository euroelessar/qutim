// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import "constants.js" as UI

Item {
    id: root
    height: 24
    property alias text: textLabel.text
    Image {
        id: image
        anchors { left: root.left; verticalCenter: root.verticalCenter; right: textLabel.left; rightMargin: 20 }
        fillMode: Image.TileHorizontally
        source: "image://theme/meegotouch-separator-background-horizontal"
    }
    Label {
        id: textLabel
        anchors { verticalCenter: root.verticalCenter; right: root.right; rightMargin: 20 }
        platformStyle: LabelStyle {
            textColor: "#8C8C8C"
            fontFamily: UI.FONT_FAMILY
            fontPixelSize: 18
        }
        font.bold: true
    }
}
