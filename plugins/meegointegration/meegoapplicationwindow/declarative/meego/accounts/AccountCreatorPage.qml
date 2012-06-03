// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
        function save() {
            label.text = "saved";
        }
        function load() {
            label.text = "loaded";
        }
    }
    Label {
        id: label
        anchors.fill: parent
        text: "Ho ho"
    }
}
