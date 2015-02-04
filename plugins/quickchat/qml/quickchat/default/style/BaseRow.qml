import QtQuick 2.3

Rectangle {
    property ItemStyle itemStyle
    readonly property bool selected: styleData !== null && styleData.selected
    readonly property bool alternate: styleData !== null && styleData.row % 2 == 1

    height: itemStyle.rowHeight
    color: selected ? itemStyle.selectedColor : alternate ? itemStyle.alternateColor : itemStyle.backgroundColor
    gradient: selected ? itemStyle.selectedGradient : alternate ? itemStyle.alternateGradient : itemStyle.backgroundGradient
}
