import QtQuick 2.0
import org.qutim 0.4

Config {
    property color backgroundColor: 'transparent'
    property color selectedColor: backgroundColor
    property color alternateColor: backgroundColor
    property var backgroundGradient
    property var selectedGradient: backgroundGradient
    property var alternateGradient: backgroundGradient

    property real rowHeight: 24
    property real rowMargin: 2

    property FontStyle font: FontStyle {
    }
}
