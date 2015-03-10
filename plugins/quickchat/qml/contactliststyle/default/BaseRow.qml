import QtQuick 2.3
import org.qutim.quickchat 0.4

Rectangle {
    id: root
    property ItemStyle itemStyle
    readonly property bool selected: styleData !== null && styleData.selected
    readonly property bool alternate: styleData !== null && styleData.row % 2 == 1

    Component {
        id: gradientComponent
        Gradient {
        }
    }

    Component {
        id: gradientStopComponent
        GradientStop {
        }
    }

    property GradientCreator gradientObject: GradientCreator {
        id: gradientCreator
        gradientComponent: gradientComponent
        gradientStopComponent: gradientStopComponent
        data: root.selected ? itemStyle.selectedGradient
                       : root.alternate ? itemStyle.alternateGradient
                           : itemStyle.backgroundGradient
    }

    height: itemStyle.rowHeight
    color: selected ? itemStyle.selectedColor : alternate ? itemStyle.alternateColor : itemStyle.backgroundColor
    gradient: gradientCreator.object
}
