// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    property string question: "Beer, wine, vodka, champagne: after which drink in this sequence I should stop?"
    property string answers: "vodka;Vodka"
    property string message: "We are ready to drink with you!"
    impl: QtObject {
        function save() {
            config.setValue("antispam/enabled", enableSwitch.checked);
            config.setValue("antispam/handleAuth", authSwitch.checked);
            config.setValue("antispam/question", questionField.text);
            config.setValue("antispam/answers", answersField.text);
            config.setValue("antispam/success", messageField.text);
        }
        function load() {
            enableSwitch.checked = config.value("antispam/enabled", false);
            authSwitch.checked = config.value("antispam/handleAuth", true);
            console.debug("!!!", config.value("antispam/question", "123"))
            questionField.text = config.value("antispam/question", qsTranslate("Antispam::SettingsWidget", root.question));
            console.debug("!!!", questionField.text)
            answersField.text = config.value("antispam/answers", qsTranslate("Antispam::SettingsWidget", root.answers));
            messageField.text = config.value("antispam/success", qsTranslate("Antispam::SettingsWidget", root.message));
        }
    }
    Config {
        id: config
    }

    FlickableColumn {
		anchors.fill: parent
        spacing: 10
        SwitchRow {
            id: enableSwitch
            text: qsTr("Enable antispam")
        }
        SwitchRow {
            id: authSwitch
            enabled: enableSwitch.checked
            text: qsTr("Handle auth requests")
        }
        Label {
            width: parent.width
            text: qsTr("Question")
        }
        TextArea {
            id: questionField
            readOnly: !enableSwitch.checked
            height: Math.max (200, implicitHeight)
            width: parent.width
        }
        Label {
            width: parent.width
            text: qsTr("Answers (semicolon as a separator)")
        }
        TextArea {
            id: answersField
            readOnly: !enableSwitch.checked
            height: Math.max (200, implicitHeight)
            width: parent.width
        }
        Label {
            width: parent.width
            text: qsTr("Message on correct answer")
        }
        TextArea {
            id: messageField
            readOnly: !enableSwitch.checked
            height: Math.max (200, implicitHeight)
            width: parent.width
        }
    }
}
