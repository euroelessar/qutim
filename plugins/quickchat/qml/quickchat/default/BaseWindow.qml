import QtQuick 2.1
import QtQuick.Controls 1.0

ApplicationWindow {
    id: window

    property var toolBarModel

    readonly property int toolBarButtonSize: 26

    Component {
        id: genericButtonComponent

        ToolButton {
            id: button
            implicitHeight: window.toolBarButtonSize
            implicitWidth: window.toolBarButtonSize

            checkable: loaderAction.checkable
            checked: loaderAction.checked
            iconName: loaderAction.iconName
            text: loaderAction.text
            tooltip: loaderAction.tooltip
            Connections {
                target: loaderAction
                onCheckedChanged: button.checked = loaderAction.checked
            }
            onClicked: loaderAction.isCheckable ? loaderAction.trigger() : loaderAction.trigger()
        }
    }

    Component {
        id: separatorComponent
        Rectangle {
            implicitHeight: window.toolBarButtonSize
            implicitWidth: 4
            color: "#ff0000"
        }
    }

    Component {
        id: toolButtonComponent

        Loader {
            id: loader
            sourceComponent: modelAction.separator ? separatorComponent : genericButtonComponent
            property QtObject loaderAction: modelAction
            Rectangle {
                z: 100
                anchors.fill: loader
                color: "#00ffff40"
            }
        }
    }

    toolBar: ToolBar {
        id: toolBar
        height: toolBarButtonSize + toolBar.contentItem.anchors.topMargin + toolBar.contentItem.anchors.bottomMargin

        Row {
            id: toolBarLayout
            spacing: 4

            Repeater {
                id: toolBarRepeater
                delegate: toolButtonComponent
                model: window.toolBarModel
            }
        }
    }
}
