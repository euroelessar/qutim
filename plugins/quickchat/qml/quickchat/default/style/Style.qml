import QtQuick 2.0

QtObject {
    id: style

    property color backgroundColor: "transparent"

    property ItemStyle account: ItemStyle {
        source: "Account.qml"
    }
    property ItemStyle group: ItemStyle {
        source: "Group.qml"
    }
    property ItemStyle contact: ItemStyle {
        source: "Contact.qml"
    }
}
