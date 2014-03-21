import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4
import org.qutim.autopaster 0.4

SettingsItem {
    id: root

    Config {
        id: config
        group: "autoPaster"
    }

    PasterItems {
        id: paster
    }

    function save() {

        config.setValue("autoSubmit", autoSubmitEdit.checked);
        config.setValue("defaultLocation", defaultLocationEdit.currentIndex);
        config.setValue("lineCount", lineCountEdit.value);
    }

    function load() {
        autoSubmitEdit.checked = config.value("autoSubmit", false);
        defaultLocationEdit.currentIndex = config.value("defaultLocation", 0);
        lineCountEdit.value = config.value("lineCount", 5);
        itemsModel.clear();

        for (var i = 0; i < paster.items.length; ++i) {
            itemsModel.append({ items: paster.items[i] });
        }
    }

    ListModel {
        id: itemsModel
    }
    GridLayout {
        anchors.fill: parent
        columns: 2
        Label {
            text: qsTr("Autosubmit:")
        }

        CheckBox {
            id: autoSubmitEdit
            Layout.fillWidth: true
            onCheckedChanged: root.modify()
        }

        Label {
            text: qsTr("Default paste:")
        }

        ComboBox {
            id: defaultLocationEdit
            model: itemsModel
            onCurrentTextChanged: root.modify()
        }

        Label {
            text: qsTr("The number of rows to trigger:")
        }

        SpinBox {
            id: lineCountEdit
            onValueChanged: root.modify()
            Layout.fillWidth: true
        }

        Item {
            Layout.fillHeight: true
        }
    }
}

