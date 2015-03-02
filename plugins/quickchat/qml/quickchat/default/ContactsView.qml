import QtQuick 2.2
import QtQuick.Controls 1.0
import org.qutim 0.4
import org.qutim.quickchat 0.4
import "constants.js" as Constants
import "details" as Details

TableView {
    id: root

    headerVisible: false
    backgroundVisible: false
    alternatingRowColors: false
    frameVisible: false

    model: flatModel

    property string searchText

    onSearchTextChanged: {
        if (contactModel.object)
            contactModel.object.setFilterFixedString(searchText)
    }

    Service {
        id: chatLayer
        name: "ChatLayer"
    }

    TableViewColumn {
    }

    readonly property ContactListStyle style: styleLoader.item

    ThemedLoader {
        id: styleLoader
        category: "contactliststyle"
    }

    Service {
        id: contactModel
        name: "ContactModel"

        onObjectChanged: {
            if (contactModel.object)
                contactModel.object.setFilterFixedString(searchText)
        }
    }

    function componentByType(type) {
        if (type === Constants.ACCOUNT)
            return accountComponent;
        if (type === Constants.GROUP)
            return groupComponent;
        if (type === Constants.CONTACT)
            return contactComponent;
        return undefined;
    }

    function stackByType(type) {
        if (type === Constants.ACCOUNT)
            return root.accountItemsStack;
        if (type === Constants.GROUP)
            return root.groupItemsStack;
        if (type === Constants.CONTACT)
            return root.contactItemsStack;
        return undefined;
    }

    property var contactItemsStack: []
    property var groupItemsStack: []
    property var accountItemsStack: []

    Component {
        id: accountComponent

        Details.ItemProxy {
            sourceComponent: root.style.accountDelegate
        }
    }

    Component {
        id: groupComponent

        Details.ItemProxy {
            sourceComponent: root.style.groupDelegate
        }
    }

    Component {
        id: contactComponent

        Details.ItemProxy {
            sourceComponent: root.style.contactDelegate
        }
    }

    FlatProxyModel {
        id: flatModel
        sourceModel: contactModel.object
    }

    itemDelegate: Item {}
    rowDelegate: Item {
        id: rowItemContainer

        property Item rowItem: null
        property var itemType: undefined
        property var modelItemType: model ? model.itemType : null

        function fallbackBindings() {
            rowItemContainer.height = 16;
        }

        function setupItem() {
            if (!model) {
                fallbackBindings();
                return;
            }

            // retrieve row item from cache
            var stack = stackByType(model.itemType);
            if (stack === undefined) {
                fallbackBindings();
                return;
            }

            if (stack.length > 0) {
                rowItem = stack.pop();
            } else {
                var component = componentByType(model.itemType);
                if (component === undefined) {
                    fallbackBindings();
                    return;
                }
                rowItem = component.createObject(root);
                if (rowItem === null) {
                    fallbackBindings();
                    return;
                }
            }
            itemType = model.itemType;

            // Bind container to item size
//            rowItemContainer.width = Qt.binding( function() { return rowItem.width });
            rowItemContainer.height = Qt.binding( function() { return rowItem.height });
            rowItem.width = Qt.binding( function() { return rowItemContainer.width });

            // Reassign row-specific bindings
            rowItem.itemStyleData = Qt.binding( function() { return styleData });
            rowItem.itemModelData = Qt.binding( function() { return modelData });
            rowItem.itemModel = Qt.binding( function() { return model });
            rowItem.parent = rowItemContainer;
            rowItem.visible = true;
        }

        function clearItem() {
            if (rowItem) {
                rowItem.visible = false;
                rowItem.parent = null;
                rowItem.width = rowItemContainer.width;
                stackByType(itemType).push(rowItem);
            }
            fallbackBindings();
            rowItem = null;
            itemType = undefined;
        }

        onModelItemTypeChanged: {
            clearItem();
            setupItem();
        }
        Component.onCompleted: setupItem()
        Component.onDestruction: clearItem()
    }

    ControlledMenu {
        id: menu
    }

    Rectangle {
        id: colorRect
        anchors.fill: parent
        color: root.style.backgroundColor
        z: -2
    }

    onActivated: {
        var data = flatModel.rowData(row);
        var contact = data.contact;

        if (contact !== undefined && contact !== null) {
            chatLayer.object.session(contact, true).activate();
        }
    }
    onClicked: {
        var data = flatModel.rowData(row);
        if (data.collapsed === true)
            flatModel.expand(row);
        else if (data.collapsed === false)
            flatModel.collapse(row);
    }

    MouseArea {
        id: mousearea

        anchors.fill: root
        acceptedButtons: Qt.RightButton

        onClicked: {
            var row = root.rowAt(mouse.x, mouse.y);
            if (row < 0)
                return;

            var data = flatModel.rowData(row);
            var contact = data.contact;

            if (contact !== undefined && contact !== null) {
                root.currentRow = row;
                menu.controller = contact;
                menu.popup();
            }
        }
    }
}
