import QtQuick 2.3

Rectangle {
    property ItemStyle itemStyle
    readonly property bool selected: styleData !== null && styleData.selected

    height: itemStyle.rowHeight
    color: selected ? itemStyle.selectedColor : itemStyle.backgroundColor
    gradient: selected ? itemStyle.selectedGradient : itemStyle.backgroundGradient
}
