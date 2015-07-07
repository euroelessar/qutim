// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

TextField {
    id: awayField
    width: parent.width
    property alias minimumValue: validator.bottom
    property alias maximumValue: validator.top
    property alias value: awayField.text
    validator: IntValidator {
        id: validator
        bottom: 1
        top: 120
    }
    inputMethodHints: Qt.ImhDigitsOnly
}
