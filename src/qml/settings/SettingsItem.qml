import QtQuick 2.1

Item {
    property string title: "Example settings item"
    property string iconSource: "image://icontheme/%1".arg(iconName)

    property string iconName
}
