// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    anchors.margins: 10
    property alias account: root.controller
    impl: QtObject {
        function save() {
            config.setValue("general/passwd", passwordField.text, Config.Crypted);
        }
        function load() {
            passwordField.text = config.value("general/passwd", "", Config.Crypted);
        }
    }
    Config {
        id: config
        object: root.account
    }
    Column {
        spacing: 10
        Label {
            width: root.width
            text: qsTr("UIN:") + " <i>" + (root.account === null ? "" : root.account.id) + "</i>"
        }
        Label {
            text: qsTr("Password:")
        }
        TextField {
            id: passwordField
            width: root.width
            echoMode: TextInput.PasswordEchoOnEdit
        }
    }

    tools: SettingsToolBarLayout {
		id: toolBarLayout
        page: root
	}
}
