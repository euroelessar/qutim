import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

Item {
    width: parent.width
    height: Math.max(labelItem.height, switchItem.height)
    property alias text: labelItem.text
    property alias checked: switchItem.checked
    Label {
        id: labelItem
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: switchItem.left
        anchors.rightMargin: 10
        text: qsTr("Set status \"Away\"")
    }
    Switch {
        id: switchItem
        anchors.right: parent.right
    }
}
