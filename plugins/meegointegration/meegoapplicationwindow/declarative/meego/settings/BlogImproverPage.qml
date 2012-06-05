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
            config.setValue("BlogImprover/enablePstoIntegration", pstoSwitch.checked);
            config.setValue("BlogImprover/enableJuickIntegration", juickSwitch.checked);
            config.setValue("BlogImprover/enableBnwIntegration", bnwSwitch.checked);
        }
        function load() {
            pstoSwitch.checked = config.value("BlogImprover/enablePstoIntegration", true);
            juickSwitch.checked = config.value("BlogImprover/enableJuickIntegration", true);
            bnwSwitch.checked = config.value("BlogImprover/enableBnwIntegration", true);
        }
    }
    Config {
        id: config
    }

    Column {
        anchors.fill: parent
        spacing: 10
        SwitchRow {
            id: pstoSwitch
            text: qsTr("Enable Psto integration")
        }
        SwitchRow {
            id: juickSwitch
            text: qsTr("Enable Juick integration")
        }
        SwitchRow {
            id: bnwSwitch
            text: qsTr("Enable Bnw integration")
        }
    }
}
