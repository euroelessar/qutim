import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4
import org.qutim.qticons 0.4

SettingsItem {
    id: root
    
    Config {
        id: config
        group: "qticons"
    }
    
    IconsList {
        id: iloader
    }

    function save() {
        config.setValue("qutim-default", defaultThemeCheckBox.checked)
        config.setValue("nasty-fix", nastyIconsFix.checked)
    }
    
    function load() {
        defaultThemeCheckBox.checked = config.value("qutim-default", false);

        nastyIconsFix.checked = config.value("nasty-fix", false);

        missingIconsBox.text = iloader.iconsList
    }
    
    ColumnLayout {
        anchors.fill: parent
        
        CheckBox {
            id: defaultThemeCheckBox
            text: qsTr("Enable qutim-default icon theme:")
            onCheckedChanged: root.modify()
        }

        CheckBox {
            id: nastyIconsFix
            text: qsTr("Enable nasty icons fix (if you do not have any icons):")
            onCheckedChanged: root.modify()
        }

        Label {
            text: qsTr("List of missing icons in your theme:")
        }

        TextArea {
            id: missingIconsBox
            readOnly: true
            Layout.fillWidth: true
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
