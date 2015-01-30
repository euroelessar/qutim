import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.qutim.mac 0.4

ApplicationWindow {
    id: root

    MacToolBar {
        id: toolBar
        window: root
    }

    property var toolBarModel
    property alias searchText: toolBar.searchText

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
