import QtQuick 2.2
import QtQuick.Controls 1.1 as Controls
import org.qutim 0.4

Controls.Menu {
    id: menu

    property alias controller: container.controller

    MenuContainer {
        id: container

        onActionAdded: {
            if (action.separator) {
                menu.insertSeparator(index);
            } else {
                var item = menuItemComponent.createObject(null, { myAction: action });
                menu.insertItem(index, item);
            }
        }
        onActionRemoved: {
            var item = menu.items[index];
            menu.removeItem(item);
        }
    }

    Action {
        id: defaultAction
    }

    Component {
        id: menuItemComponent

        Controls.MenuItem {
            property Action myAction

            function tryAction() {
                return myAction ? myAction : defaultAction;
            }

            checkable: tryAction().checkable
            checked: tryAction().checked
            enabled: tryAction().enabled
            iconName: tryAction().iconName
            iconSource: tryAction().iconSource
            text: tryAction().text
            visible: tryAction().visible

            onToggled: tryAction().checked = checked
            onTriggered: tryAction().trigger()
        }
    }
}
