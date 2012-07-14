// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    property string backendType
    impl: QtObject {
        function save() {
            for (var i = 0; i <= Notification.LastType; ++i) {
                var item = repeater.itemAt(i);
                var key = "notification/" + notifications.typeName(i) + "/" + backendType;
                config.setValue(key, item.checked);
            }
        }
        function load() {
            for (var i = 0; i <= Notification.LastType; ++i) {
                var item = repeater.itemAt(i);
                var key = "notification/" + notifications.typeName(i) + "/" + backendType;
                item.checked = config.value(key, false);
            }
        }
    }
    Config {
        id: config
    }
    Notifications {
        id: notifications
    }
    
    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: column.width
        contentHeight: column.height
        Column {
            id: column
            width: flickable.width
            spacing: 10
            
            Repeater {
                id: repeater
                model: Notification.LastType + 1
                
                SwitchRow {
                    id: switchRow
                    text: notifications.typeText(index)
                }
            }
        }
    }
}
