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
            config.setValue("urlPreview/imagesPreview", imagesSwitch.checked);
            config.setValue("urlPreview/youtubePreview", youtubeSwitch.checked);
            config.setValue("urlPreview/HTML5Audio", audioSwitch.checked);
            config.setValue("urlPreview/HTML5Video", videoSwitch.checked);
        }
        function load() {
            imagesSwitch.checked = config.value("urlPreview/imagesPreview", true);
            youtubeSwitch.checked = config.value("urlPreview/youtubePreview", true);
            audioSwitch.checked = config.value("urlPreview/HTML5Audio", true);
            videoSwitch.checked = config.value("urlPreview/HTML5Video", true);
        }
    }
    Config {
        id: config
    }

    Column {
		anchors.fill: parent
        spacing: 10
        SwitchRow {
            id: imagesSwitch
            text: qsTr("Images preview")
        }
        SwitchRow {
            id: youtubeSwitch
            text: qsTr("YouTube preview")
        }
        SwitchRow {
            id: audioSwitch
            text: qsTr("HTML5 Audio")
        }
        SwitchRow {
            id: videoSwitch
            text: qsTr("HTML5 Video")
        }
    }
}
