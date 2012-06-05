// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
        property variant list: []
        function save() {
            var children = column.children;
            for (var i = 0; i < children.length; ++i) {
                if (children[i].save)
                    children[i].save();
            }
        }
        function load() {
            var children = column.children;
            for (var i = 0; i < children.length; ++i) {
                if (children[i].load)
                    children[i].load();
            }
        }
    }
    property QtObject contactList: serviceManager.contactList
    ServiceManager {
		id: serviceManager
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
                model: root.contactList.accounts
                ItemDelegate {
                    id: objectItem
                    property QtObject account: modelData
                    title: account.name === "" ? account.id : account.name
                    subtitle: account.id
                    property variant model: ({})
                    function load() { switchItem.checked = config.value("autoConnect", true); }
                    function save() { config.setValue("autoConnect", switchItem.checked); }
                    property QtObject config: Config {
                        object: objectItem.account
                    }
                    Switch {
                        id: switchItem
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            rightMargin: 5
                        }
                    }
                }
            }
        }
    }
}
