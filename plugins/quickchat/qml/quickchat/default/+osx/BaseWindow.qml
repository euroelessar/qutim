import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.qutim.mac 0.4

ApplicationWindow {
    id: root
    color: "transparent"

    property var toolBarModel
    property alias searchText: toolBar.searchText
    property alias backgroundColor: blur.backgroundColor

    MacToolBar {
        id: toolBar
        window: root
    }

    WindowBackgroundBlur {
        id: blur
        window: root
        radius: 25
        backgroundColor: "#a0f7fafc"
    }

    Repeater {
        model: root.toolBarModel
        delegate: Item {
            visible: false
            property QtObject action: modelAction
            property bool added: false
//            onActionChanged: if (added) toolBar.replace(index, modelAction)
        }

        onItemAdded: {
            toolBar.insert(index, item.action);
            item.added = true;
        }
        onItemRemoved: {
            toolBar.remove(index);
            item.added = false;
        }
    }
}
