import QtQuick 2.0

Item {
    Item {
        id: root
        anchors.fill: parent
        anchors.margins: root.itemStyle.rowMargin

        property ItemStyle itemStyle: viewStyle.group

        Loader {
            id: image
//            sourceComponent: viewStyle.groupTriangle
            width: height
            anchors {
                left: root.left
                top: root.top
                bottom: root.bottom
                leftMargin: 2
            }
        }

        Label {
            anchors {
                left: image.right
                right: root.right
                verticalCenter: root.verticalCenter
                leftMargin: 4
            }
            text: model === null ? 'null' : model.title
            itemStyle: root.itemStyle
        }
    }
}
