import QtQuick 2.1

Item {
    id: item

    property string title: "Example settings item"
    property string iconSource: "image://icontheme/%1".arg(iconName)
    property string iconName

    property Component component
}
