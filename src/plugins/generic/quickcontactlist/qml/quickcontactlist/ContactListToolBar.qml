import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

ToolBar {
    Layout.fillWidth: true
    
    Service {
        id: contactList
        name: "ContactList"
    }

    ListModel {
        id: actionsModel
    }

    ActionContainer {
        controller: contactList.object.buttons
        visible: true

        onActionAdded: {
            console.log('action added', index, action.iconName, action.text, action)
            actionsModel.insert(index, { 'modelAction': action })
        }
        onActionRemoved: actionsModel.remove(index)
    }

    RowLayout {
        ToolButton {
            iconName: "show-menu"
            text: qsTr("Show menu")
            menu: mainMenu
            
            DynamicMenu {
                id: mainMenu
                controller: contactList.object
            }
        }

        Repeater {
            model: actionsModel

            ToolButton {
                id: button
                checkable: modelAction.checkable
                checked: modelAction.checked
                iconName: modelAction.iconName
                text: modelAction.text
                tooltip: modelAction.tooltip
                
                Connections {
                    target: modelAction
                    onCheckedChanged: button.checked = modelAction.checked
                }

                onClicked: modelAction.isCheckable ? modelAction.trigger() : modelAction.trigger()
            }
        }
    }
}
