import QtQuick 2.0

Text {
    property FontStyle fontStyle
    readonly property bool selected: styleData !== null && styleData.selected

    color: selected ? fontStyle.highlightColor : fontStyle.textColor
    renderType: Text.NativeRendering
    textFormat: Text.PlainText
    elide: Text.ElideRight
    maximumLineCount: 1
    verticalAlignment: Text.AlignVCenter

    font.weight: fontStyle.fontWeight
    font.family: fontStyle.fontFamily
    font.italic: fontStyle.fontItalic
    font.pointSize: fontStyle.fontPointSize
}
