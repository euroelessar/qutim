import QtQuick 2.2
import QtQuick.Controls 1.0
import org.qutim 0.4
import org.qutim.quickchat 0.4
import "constants.js" as Constants

TableView {
    id: root

    headerVisible: false
    backgroundVisible: false
    alternatingRowColors: false

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

    readonly property Style style: styleLoader.item

    Loader {
        id: styleLoader
        source: 'style/Style.qml'
    }

    Service {
        id: contactModel
        name: "ContactModel"

        onObjectChanged: {
            if (contactModel.object)
                contactModel.object.setFilterFixedString(searchText)
        }
    }

    function rowByType(type) {
        if (type === Constants.ACCOUNT)
            return root.style.rowAccountDelegate;
        if (type === Constants.GROUP)
            return root.style.rowGroupDelegate;
        if (type === Constants.CONTACT)
            return root.style.rowContactDelegate;
        return '';
    }

    function itemByType(type) {
        if (type === Constants.ACCOUNT)
            return root.style.itemAccountDelegate;
        if (type === Constants.GROUP)
            return root.style.itemGroupDelegate;
        if (type === Constants.CONTACT)
            return root.style.itemContactDelegate;
        return '';
    }

    FlatProxyModel {
        id: flatModel
        sourceModel: contactModel.object
    }

    itemDelegate: Loader {
        source: model === null ? '' : itemByType(model.itemType)
        height: item ? item.height : 16

        readonly property Style viewStyle: root.style
    }
    rowDelegate: Loader {
        source: model === null ? '' : rowByType(model.itemType)
        height: item ? item.height : 16

        readonly property Style viewStyle: root.style
        readonly property var model: flatModel.rowData(styleData.row)
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
