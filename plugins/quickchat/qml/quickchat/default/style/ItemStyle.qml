import QtQuick 2.0

QtObject {
    property url source

    property color backgroundColor: 'transparent'
    property color selectedColor: backgroundColor
    property color alternateColor: backgroundColor
    property Gradient backgroundGradient
    property Gradient selectedGradient: backgroundGradient
    property Gradient alternateGradient: backgroundGradient

    property real rowHeight: 24
    property real rowMargin: 2

    property FontStyle font: FontStyle {
    }
}
