import QtQuick 2.2
import QtQuick.Controls 1.1 as Controls
import org.qutim 0.4

Controls.Menu {
    id: menu
    
    property alias controller: container.controller
    
    ActionContainer {
        id: container
        
        onActionAdded: {
            console.log('action added', index, action.text)
            var item = menuItemComponent.createObject(menu, { myAction: action })
            menu.insertItem(index, item);
        }
        onActionRemoved: {
            var item = menu.items[index];
            console.log('action removed', index, item.text)
            menu.removeItem(item)
            item.destroy();
        }
    }
    
    Component {
        id: menuItemComponent
        
        Controls.MenuItem {
            property Action myAction
            
            checkable: myAction.checkable
            checked: myAction.checked
            enabled: myAction.enabled
            iconName: myAction.iconName
            iconSource: myAction.iconSource
            text: myAction.text
            visible: myAction.visible
            
            onToggled: myAction.checked = checked
            onTriggered: myAction.trigger()
        }
    }
}
