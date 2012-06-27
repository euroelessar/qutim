// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
        function save() {
            config.setValue("away-enabled", awayCheckBox.checked);
            config.setValue("na-enabled", naCheckBox.checked);
            config.setValue("away-secs", parseInt(awayField.text) * 60);
            config.setValue("na-secs", parseInt(naField.text) * 60);
        }
        function load() {
            awayCheckBox.checked = config.value("away-enabled", true);
            naCheckBox.checked = config.value("na-enabled", true);
            awayField.text = config.value("away-secs", 180) / 60;
            naField.text = config.value("na-secs", 600) / 60;
        }
    }
    acceptableInput: awayField.acceptableInput && naField.acceptableInput
    Config {
        id: config
        path: "auto-away"
    }

    Column {
		anchors.fill: parent
        spacing: 10
        SwitchRow {
            id: awayCheckBox
            text: qsTr("Set status \"Away\" in %n minutes", "", parseInt(awayField.text))
        }
        TextField {
            id: awayField
            readOnly: !awayCheckBox.checked
            validator: IntValidator {
                bottom: 1
                top: 120
            }
            inputMethodHints: Qt.ImhDigitsOnly
        }
        SwitchRow {
            id: naCheckBox
            text: qsTr("Set status \"Not available\" in %n minutes", "", parseInt(naField.text))
        }
        TextField {
            id: naField
            readOnly: !naCheckBox.checked
            validator: IntValidator {
                bottom: 1
                top: 120
            }
            inputMethodHints: Qt.ImhDigitsOnly
        }
    }
}
