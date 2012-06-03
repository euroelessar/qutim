// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    anchors.margins:10
    impl: QtObject {
        function save() {
            config.setValue("away-enabled", awayCheckBox.checked);
            config.setValue("na-enabled", naCheckBox.checked);
            config.setValue("away-secs", awaySlider.value * 60);
            config.setValue("na-secs", naSlider.value * 60);
        }
        function load() {
            awayCheckBox.checked = config.value("away-enabled", true);
            naCheckBox.checked = config.value("na-enabled", true);
            awaySlider.value = config.value("away-secs", 180) / 60;
            naSlider.value = config.value("na-secs", 600) / 60;
        }
    }
    Config {
        id: config
        path: "auto-away"
    }

    Column {
		anchors.fill: parent
        spacing: 10
        Row {
			spacing: 10
			Label { text: qsTr("Set status \"Away\"") }
			Switch { id: awayCheckBox; anchors.right: parent.right; }
		}
        Slider {
            id: awaySlider
            stepSize: 1
            valueIndicatorVisible: true
            valueIndicatorText: qsTr("%n minutes", "", value);
            minimumValue: 1
            maximumValue: 120
            enabled: awayCheckBox.checked
        }
        Row {
			spacing: 10
			Label { text: qsTr("Set status \"Not available\"") }
            Switch { id: naCheckBox; anchors.right: parent.right; }
		}
        Slider {
            id: naSlider
            stepSize: 1
            valueIndicatorVisible: true
            valueIndicatorText: qsTr("%n minutes", "", value);
            minimumValue: 1
            maximumValue: 120
            enabled: naCheckBox.checked
        }
    }
}
