// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

ButtonStyle {
    property variant __enabled: comboBox.enabled ? "" : "-disabled"
    property variant __pressed: comboBox.pressed ? "-pressed" : ""
    property string indicatorSource: "image://theme/meegotouch-combobox-indicator"
                                     + __invertedString + __enabled + __pressed
    
    property int subfontWeight: Font.Normal
    // Subtext Color
    property color subtextColor: pressedSubtextColor
    property color pressedSubtextColor: pressedTextColor
    property color disabledSubtextColor: disabledTextColor
    property color checkedSubtextColor: checkedTextColor
    
    // Background Color
    property color itemBackgroundColor: "transparent"
    property color itemSelectedBackgroundColor: "#3D3D3D"
}
