// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    property QtObject object
    impl: QtObject {
        function save() {
            root.object.save(dataForm.item);
        }
        function load() {
            root.object.controller = root.controller;
            root.object.load();
        }
    }
    
    Flickable {
        id: flickable
		anchors.fill: parent
        contentWidth: dataForm.width
        contentHeight: dataForm.height
            
        DataForm {
            id: dataForm
            width: flickable.width
            item: root.object.item
        }
    }
}
