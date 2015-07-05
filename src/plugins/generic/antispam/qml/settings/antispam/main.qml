import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root

    Config {
        id: config
        group: "antispam"
    }

    function save() {

        config.setValue("enabled", enabledBoxEdit.checked);
        config.setValue("handleAuth", handleAuthEdit.checked);
        config.setValue("answers", answerEdit.text);
        config.setValue("success", successEdit.text);
        config.setValue("question", questionEdit.text);
    }

    function load() {
        enabledBoxEdit.checked = config.value("enabled", true);
        handleAuthEdit.checked = config.value("handleAuth", true);
        answerEdit.text = config.value("answers", "");
        successEdit.text = config.value("success", "");
        questionEdit.text = config.value("question", "");
    }

    GridLayout {
        id: grid
        anchors.fill: parent
        columns: 1

        CheckBox {
            id: enabledBoxEdit
            text: qsTr("Enabled")
            Layout.fillWidth: true
            onCheckedChanged: root.modify()
        }
        CheckBox {
            id: handleAuthEdit
            text: qsTr("Handle auth requests")
            Layout.fillWidth: true
            enabled: enabledBoxEdit.checked
            onCheckedChanged: root.modify()
        }

        Label {
            text: "Question:"
        }

        TextArea {
            id: questionEdit
            Layout.fillWidth: true
            enabled: enabledBoxEdit.checked
            onTextChanged: root.modify()
        }

        Label {
            text: "Answers (semicolon as a separator):"
        }

        TextArea {
            id: answerEdit
            Layout.fillWidth: true
            enabled: enabledBoxEdit.checked
            onTextChanged: root.modify()
        }

        Label {
            text: "Message on correct answer:"
        }

        TextArea {
            id: successEdit
            Layout.fillWidth: true
            enabled: enabledBoxEdit.checked
            onTextChanged: root.modify()
        }
    }
}

