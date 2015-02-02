import QtQuick 2.0

QtObject {
    id: style

    property color backgroundColor: "transparent"

    property url rowAccountDelegate: 'BaseRow.qml'
    property url rowGroupDelegate: 'BaseRow.qml'
    property url rowContactDelegate: 'BaseRow.qml'
    property url itemAccountDelegate: 'BaseItem.qml'
    property url itemGroupDelegate: 'BaseItem.qml'
    property url itemContactDelegate: 'BaseItem.qml'
}
