import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root
    
    Config {
        id: config
        group: "qticons"
    }

    Service {
        id: iloader
        name: "IconLoader"
    }

    function save() {
        config.setValue("qutim-default", defaultThemeCheckBox.checked)
    }
    
    function load() {
        defaultThemeCheckBox.checked = config.value("qutim-default", true);

        missingIconsBox.text = iloader.object.iconsList;
    }
    
    ColumnLayout {
        anchors.fill: parent
        
        CheckBox {
            id: defaultThemeCheckBox
            text: qsTr("Enable qutim-default icon theme")
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
