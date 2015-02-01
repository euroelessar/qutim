import QtQuick 2.2
import QtQuick.Controls 1.0
import org.qutim 0.4
import org.qutim.quickchat 0.4
import "constants.js" as Constants
import "style"

ScrollView {
    id: root

    property string searchText

    onSearchTextChanged: {
        if (contactModel.object)
            contactModel.object.setFilterFixedString(searchText)
    }

    ListView {
        id: listView

        Service {
            id: chatLayer
            name: "ChatLayer"
        }

        Style {
            id: style
        }

        Service {
            id: contactModel
            name: "ContactModel"

            onObjectChanged: {
                if (contactModel.object)
                    contactModel.object.setFilterFixedString(searchText)
            }
        }

        function styleByType(type) {
            if (type === Constants.ACCOUNT)
                return style.account;
            if (type === Constants.GROUP)
                return style.group;
            if (type === Constants.CONTACT)
                return style.contact;
            return undefined;
        }

        model: flatModel

        FlatProxyModel {
            id: flatModel
            sourceModel: contactModel.object
        }

        delegate: Loader {
            id: loader
            source: loader.style.source
            width: listView.width

            property ItemStyle style: listView.styleByType(itemType)
            property var itemModel: model
            property QtObject styleData: QtObject {
            }
        }

        ControlledMenu {
            id: menu
        }

        Rectangle {
            id: colorRect
            anchors.fill: parent
            color: style.backgroundColor
            z: -2
        }

        MouseArea {
            id: mousearea

            z: -1
            anchors.fill: listView
            propagateComposedEvents: true

            function mouseIndex(mouse) {
                return listView.indexAt(0, mouse.y + listView.contentY);
            }

            function mouseItem(mouse) {
                return listView.itemAt(0, mouse.y + listView.contentY);
            }

            function mouseContact(mouse) {
                return mouseItem(mouse).itemModel.contact;
            }

            onClicked: {
                var index = mouseIndex(mouse);
                var contact = mouseContact(mouse);

                if (mouse.button === Qt.RightButton && index >= 0 && contact !== undefined && contact !== null) {
                    listView.currentIndex = index;
                    menu.controller = contact;
                    menu.popup();
                } else if (mouse.button === Qt.LeftButton && index >= 0) {
                    listView.currentIndex = index;
                }
            }

            onDoubleClicked: {
                var index = mouseIndex(mouse);
                var contact = mouseContact(mouse);

                if (mouse.button === Qt.LeftButton && index >= 0 && contact !== undefined && contact !== null) {
                    chatLayer.object.session(contact, true).activate();
                }
            }

            onPressed: {
                root.forceActiveFocus();
            }
        }
    }
}
