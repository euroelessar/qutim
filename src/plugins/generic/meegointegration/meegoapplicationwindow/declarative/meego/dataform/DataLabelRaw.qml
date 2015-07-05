// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: root
    width: parent.width
    height: label.height
    property string title
    property string text
    Label {
        id: label
        textFormat: Text.RichText
        text: "<font color=\"#8C8C8C\">" + root.title + ":</font> <b>" + root.text + "</b>"
        width: parent.width
    }
}
