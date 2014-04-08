import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root
    
    Config {
        id: config
        group: "yandexNarod"
    }
    
    function save() {
        config.setValue("login", loginField.text);
        config.setValue("passwd", passwordField.text);
    }
    
    function load() {
        loginField.text = config.value("login");
        passwordField.text = config.value("passwd");  ///How i can use the crypted values
    }

    GridLayout {
        anchors.fill: parent
        columns: 2

        Label {
            text: qsTr("Login")
        }
        TextField {
            id: loginField
        }

        Label {
            text: qsTr("Login")
        }
        TextField {
            id: passwordField

            echoMode: TextInput.Password
        }
    }
}

