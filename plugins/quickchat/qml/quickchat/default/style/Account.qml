import QtQuick 2.0

Item {
    Item {
        id: root
        anchors.fill: parent
        anchors.margins: root.itemStyle.rowMargin

        property ItemStyle itemStyle: viewStyle.account

        Label {
            anchors.fill: parent
            text: model === null ? 'null' : model.title
            itemStyle: root.itemStyle
        }
    }
}
