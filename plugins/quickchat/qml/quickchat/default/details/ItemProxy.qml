import QtQuick 2.0

Item {
    id: root
    height: loader.height

    property var itemModel: null
    property var itemModelData: null
    property QtObject itemStyleData: null
    property alias sourceComponent: loader.sourceComponent

    Loader {
        id: loader
        height: item ? item.height : 16
        width: parent.width

        readonly property var model: root.itemModel
        readonly property var modelData: root.itemModelData
        readonly property QtObject styleData: root.itemStyleData
    }
}
