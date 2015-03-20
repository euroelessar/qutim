import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Controls.Forms 1.0
import org.qutim 0.4

SettingsItem {
    id: root
    
    property var iconOptions: [
        { "name": qsTr("Show number of new messages") },
        { "name": qsTr("Show number of chats with new messages") },
        { "name": qsTr("Show only icon") }
    ]
    property int currentOption: 0
    
    function save() {
        config.setValue("showNumber", currentOption);
        config.setValue("blink", blinkIconOption.checked);
        config.setValue("showIcon", showIconOption.checked);
    }
    
    function load() {
        root.currentOption = config.value("showNumber", 0);
        blinkIconOption.checked = config.value("blink", true);
        showIconOption.checked = config.value("showIcon", true);
    }

    ExclusiveGroup { id: iconGroup }
    Config {
        id: config
        path: "simpletray"
    }
    
    FormLayout {
        anchors.fill: parent

        GroupBox {
            title: qsTr("Icon")
            Layout.fillWidth: true

            ColumnLayout {
                Repeater {
                    model: root.iconOptions
                    RadioButton {
                        text: modelData.name

                        onCheckedChanged: { root.currentOption = index; }

                        exclusiveGroup: iconGroup
                        checked: index === root.currentOption
                    }
                }
            }
        }
        FormLabel { text: qsTr("Other") }

        CheckBox {
            id: showIconOption
            text: qsTr("Show mail icon if there are new messages")
        }
        CheckBox {
            id: blinkIconOption
            text: qsTr("Blink icon")
        }
    }
}
