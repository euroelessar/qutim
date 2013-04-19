import QtQuick 2.1

Item {
    ContactList {
        id: contactList
    }

    Component.onCompleted: {
        contactList.show();
    }
}
