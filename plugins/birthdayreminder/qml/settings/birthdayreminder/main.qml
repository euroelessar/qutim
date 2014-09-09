import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root

    Config {
        id: config
        group: "birthdayReminder"
    }

    function save() {
        config.setValue("intervalBetweenNotifications", intervalBetweenNotificationsEdit.value);
        config.setValue("daysBeforeNotification", daysBeforeNotificationEdit.value);

    }

    function load() {
        intervalBetweenNotificationsEdit.value = config.value("intervalBetweenNotificationsEdit", 24.0);
        daysBeforeNotificationEdit.value = config.value("daysBeforeNotificationEdit", 3);
    }

    ListModel {
        id: itemsModel
    }
    GridLayout {
        anchors.fill: parent
        columns: 3
        Label {
            text: qsTr("Show notifications every: ")
        }

        SpinBox {
            id: intervalBetweenNotificationsEdit
            decimals: 1
            onValueChanged: root.modify()
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("hours.")
        }

        Label {
            text: qsTr("starting from: ")
        }

        SpinBox {
            id: daysBeforeNotificationEdit
            onValueChanged: root.modify()
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("days before birthday.")
        }

        Item {
            Layout.fillHeight: true
        }
    }
}

