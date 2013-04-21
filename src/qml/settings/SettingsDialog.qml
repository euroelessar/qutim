import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtQuick 2.1

import org.qutim.core 0.4
import org.qutim.icons 0.4

import "."

ApplicationWindow {
    id: root

    property list<SettingsItem> items
    property Item initialItem: currentItem

    readonly property SettingsItem currentItem: items[__currentIndex]

    default property alias __items : root.items
    property int __currentIndex: 0

    function __replace(settingsItem) {
        // remember old item
        var oldItem = stack.currentItem;

        __currentIndex = __indexOf(settingsItem);
        var component = settingsItem.component;
        var item = component.createObject(stack);

        stack.push(item, {}, false, true);
        if (oldItem) {
            // remove old settings item on end of transition
            stack.onBusyChanged.connect = function() {
                oldItem.destroy(0);
            }
        }
    }

    // list<SettingsItem> doesn't have indexOf method
    function __indexOf(item) {
        var index = -1;
        for (var i = 0; i !== items.count; i++) {
            if (items[i] === item) {
                index = i;
                break;
            }
        }
        return index;
    }

    Component.onCompleted: {
        __replace(initialItem);
    }

    width: 600
    height: 400

    modality: Qt.ApplicationModal
    title: qsTr("qutIM settings - %1").arg(currentItem ? currentItem.title : 'Not yet implemented')

    //toolBar: ToolBar {
    //    width: parent.width
    //    RowLayout {
    //        Repeater {
    //            model: root.__items

    //            ToolButton {
    //                onClicked: stack.replace(modelData)

    //                iconSource: modelData.iconSource
    //                text: modelData.title
    //                checked: modelData == currentItem
    //            }
    //        }
    //    }
    //}

    Rectangle {
        id: sideBar

        color: systemPalette.base
        width: 150

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            margins: 6
        }

        ScrollView {
            id: scrollView
            frameVisible: true
            anchors.fill: parent
            clip: true

            ListView {
                id: itemsView
                anchors.fill: parent

                model: items
                currentIndex: root.__currentIndex

                highlight: Rectangle {
                    z: currentItem.z
                    width: itemsView.width
                    radius: 6
                    color: systemPalette.highlight
                }
                delegate: Rectangle {
                    id: rectangle

                    property bool checked: index === ListView.view.currentIndex

                    signal clicked

                    onClicked: {
                        if (modelData !== root.currentItem) {
                            __replace(modelData);
                        }
                    }

                    width: ListView.view.width
                    height: childrenRect.height

                    //color: (!checked && index % 2) ? systemPalette.alternateBase : "transparent"
                    color: 'transparent'

                    Column {
                        id: column

                        width: parent.width

                        spacing: 3

                        Image {
                            anchors.horizontalCenter: parent.horizontalCenter

                            width: 48
                            height: 48
                            source: modelData.iconSource
                        }

                        Label {
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter

                            text: modelData.title
                            color: rectangle.checked ? systemPalette.highlightedText : systemPalette.windowText
                        }
                    }

                    MouseArea {
                        id: area

                        onClicked: parent.clicked()

                        anchors.fill: parent
                    }
                }
            }
        }
    }

    StackView {
        id: stack

        clip: true

        anchors {
            top: sideBar.top
            bottom: sideBar.bottom
            left: sideBar.right
            right: parent.right

            leftMargin: sideBar.anchors.leftMargin
            rightMargin: sideBar.anchors.rightMargin
        }
    }
}
