import QtQuick 2.0

Item {
    Item {
        id: root
        anchors.fill: parent
        anchors.margins: itemStyle.rowMargin

        property ItemStyle itemStyle: viewStyle.contact

        Image {
            id: avatar
            width: height
            anchors {
                left: root.left
                top: root.top
                bottom: root.bottom
            }
            source: model && model.iconSource && height > 0 ? model.iconSource + '&size=' + height : ''
        }

        Label {
            id: titleText
            anchors {
                left: avatar.right
                right: root.right
                top: subtitleText.visible ? root.top : undefined
                verticalCenter: subtitleText.visible ? undefined : root.verticalCenter
                leftMargin: 4
            }
            text: model === null ? 'null' : model.title
            itemStyle: root.itemStyle
        }

        Label {
            id: subtitleText
            anchors {
                left: avatar.right
                right: root.right
                top: titleText.bottom
                bottom: root.bottom
                leftMargin: 4
            }
            visible: text.length > 0
            text: model === null || model.contact === undefined ? '' : model.contact.status.text
            itemStyle: root.itemStyle
            font.pointSize: titleText.font.pointSize - 4
        }
    }
}
