import QtQuick 2.3

Rectangle {
    property ItemStyle itemStyle

    height: itemStyle.rowHeight
    color: styleData.selected ? itemStyle.selectedColor : itemStyle.backgroundColor
    gradient: styleData.selected ? itemStyle.selectedGradient : itemStyle.backgroundGradient
}
