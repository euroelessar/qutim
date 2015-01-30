import QtQuick 2.2
import org.qutim 0.4
import org.qutim.quickchat 0.4
import "constants.js" as Constants

ListView {
    id: root

    property string searchText

    Service {
        id: contactModel
        name: "ContactModel"

        onObjectChanged: {
            if (contactModel.object)
                contactModel.object.setFilterFixedString(searchText)
        }
    }

    onSearchTextChanged: {
        if (contactModel.object)
            contactModel.object.setFilterFixedString(searchText)
    }

    Service {
        id: chatLayer
        name: "ChatLayer"
    }

    function componentByType(type) {
        if (type === Constants.ACCOUNT)
            return "style/Account.qml";
        if (type === Constants.GROUP)
            return "style/Group.qml";
        if (type === Constants.CONTACT)
            return "style/Contact.qml";
        return undefined;
    }

    model: flatModel

    FlatProxyModel {
        id: flatModel
        sourceModel: contactModel.object
    }

    delegate: Loader {
        source: componentByType(itemType)
        width: root.width

        MouseArea {
            enabled: contact !== undefined && contact !== null
            anchors.fill: parent
            acceptedButtons: Qt.RightButton | Qt.LeftButton

            onClicked: {
                if (mouse.button == Qt.RightButton) {
                    root.currentIndex = model.index;
                    menu.controller = contact;
                    menu.popup();
                } else {
                    root.currentIndex = model.index;
                }
            }

            onDoubleClicked: {
                if (mouse.button == Qt.LeftButton) {
                    chatLayer.object.session(contact, true).activate();
                }
            }
        }
    }

    ControlledMenu {
        id: menu
    }
}
