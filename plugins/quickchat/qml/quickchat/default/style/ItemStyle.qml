import QtQuick 2.0

QtObject {
    property url source

    property color backgroundColor: 'transparent'
    property color selectedColor: 'transparent'
    property Gradient backgroundGradient
    property Gradient selectedGradient

    property real rowHeight: 24
    property real rowMargin: 2

    property int fontWeight: Font.Normal
    property string fontFamily: 'Helvetica'
    property bool fontItalic: false
    property real fontPointSize: 13
    property color textColor: 'black'
    property color highlightColor: 'white'
}
