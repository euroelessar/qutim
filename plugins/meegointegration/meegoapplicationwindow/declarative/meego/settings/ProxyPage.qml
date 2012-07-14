// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
        function save() {
            var account = root.controller;
            var currentName = typeBox.currentItem.data;
            var disabled = false;
            var useGlobal = false;
            if (currentName === "disabled")
                disabled = true;
            else if (currentName === "global")
                useGlobal = true;
            var index = typeBox.currentIndex - 1;
            if (account)
                --index;
            proxyHelper.save(dataForm.item, index, disabled, useGlobal);
        }
        function load() {
            typeBox.model.clear();
            var account = root.controller;
            var data = proxyHelper.load();
            var currentName = data.currentName;
            var disabled = data.disabled;
            var useGlobal = data.useGlobal;
            typeBox.model.append({ name: qsTr("Do not use proxy"), data: "disabled" });
            if (account)
                typeBox.model.append({ name: qsTr("Use global proxy"), data: "global" });
            if (disabled)
                typeBox.currentIndex = 0;
            else if (useGlobal)
                typeBox.currentIndex = 1;
            for (var i = 0; i < proxyHelper.count(); ++i) {
                typeBox.model.append({
                                         name: proxyHelper.description(i),
                                         data: proxyHelper.name(i),
                                         item: proxyHelper.settings(i)
                                     });
                if (!disabled && !useGlobal && proxyHelper.name(i) === currentName)
                    typeBox.currentIndex = typeBox.model.count - 1;
            }
        }
    }
    
    ProxyHelper {
        id: proxyHelper
        account: root.controller
    }
    
    Flickable {
        id: flickable
		anchors.fill: parent
        contentWidth: wrapper.width
        contentHeight: wrapper.height
        Item {
            id: wrapper
            width: flickable.width
            height: typeBox.height + dataForm.height

            ComboBox {
                id: typeBox
                width: parent.width
                title: qsTr("Proxy type")
            }
            
            DataForm {
                id: dataForm
                anchors { top: typeBox.bottom }
                width: parent.width
                item: typeBox.currentItem.item
            }
        }
    }
}
