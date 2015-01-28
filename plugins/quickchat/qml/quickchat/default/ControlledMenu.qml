import QtQuick 2.2
import QtQuick.Controls 1.1 as Controls
import org.qutim 0.4

Controls.Menu {
    id: menu

    property alias controller: container.controller
    property var objects: []

    MenuContainer {
        id: container
        onActionAdded: {
            if (action.separator) {
                menu.insertSeparator(index);
                menu.objects.splice(index, 0, null);
            } else {
                var item = menuItemComponent.createObject(null, { myAction: action });
                menu.insertItem(index, item);
                menu.objects.splice(index, 0, item);
            }
        }
        onActionRemoved: {
            var item = menu.items[index];
            menu.objects.splice(index, 1);
            menu.removeItem(item);
        }
    }

    Action {
        id: defaultAction
        text: '<NULL Action>'
    }

    Component {
        id: menuItemComponent

        Controls.MenuItem {
            property Action myAction: null

            function tryAction() {
                return myAction !== null ? myAction : defaultAction;
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
