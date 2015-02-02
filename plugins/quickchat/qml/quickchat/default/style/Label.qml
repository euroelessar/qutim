import QtQuick 2.0

Text {
    property ItemStyle itemStyle

    color: styleData.selected ? itemStyle.highlightColor : itemStyle.textColor
    renderType: Text.NativeRendering
    textFormat: Text.PlainText
    elide: Text.ElideRight
    maximumLineCount: 1
    verticalAlignment: Text.AlignVCenter

    font.weight: itemStyle.fontWeight
    font.family: itemStyle.fontFamily
    font.italic: itemStyle.fontItalic
    font.pointSize: itemStyle.fontPointSize
}
