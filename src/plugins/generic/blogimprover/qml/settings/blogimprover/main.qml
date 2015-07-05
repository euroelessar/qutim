import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root
    
    Config {
        id: config
        group: "BlogImprover"
    }
    
    function save() {
        config.setValue("enablePointIntegration", enablePointIntegration.checked);
        config.setValue("enableJuickIntegration", enableJuickIntegration.checked);
        config.setValue("enableBnwIntegration", enableBnwIntegration.checked);
    }
    
    function load() {
        enablePointIntegration.checked = config.value("enablePointIntegration", true);
        enableJuickIntegration.checked = config.value("enableJuickIntegration", true);
        enableBnwIntegration.checked = config.value("enableBnwIntegration", true);
    }
    GridLayout {
        anchors.fill: parent
        columns: 2
        CheckBox {
            id: enablePointIntegration
            text: qsTr("Enable Point integration")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        CheckBox {
            id: enableJuickIntegration
            text: qsTr("Enable Juick integration")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        CheckBox {
            id: enableBnwIntegration
            text: qsTr("Enable Bnw integration")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        Item {
            Layout.fillHeight: true
        }
    }
}

