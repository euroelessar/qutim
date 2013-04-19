import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick 2.1

import org.qutim.core 0.4
import org.qutim.icons 0.4

import "."

ApplicationWindow {
    id: root

    property list<Item> items
    property alias currentItem: stack.currentItem
    property Item initialItem: items[0]

    Component.onCompleted: {
        stack.push(initialItem);
        console.log(toolBar.parent.height);
    }

    width: 600
    height: 400

    modality: Qt.ApplicationModal
    title: qsTr("qutIM settings - %1").arg(currentItem ? currentItem.title : 'Not yet implemented')

    toolBar: ToolBar {
        width: parent.width
        RowLayout {
            Repeater {
                model: root.items

                ToolButton {
                    onClicked: stack.replace(modelData)

                    iconSource: modelData.iconSource
                    text: modelData.title
                    checked: modelData == currentItem
                }
            }
        }
    }

    StackView {
        id: stack
    }
}
