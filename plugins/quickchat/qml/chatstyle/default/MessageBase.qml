import QtQuick 2.3
import org.qutim 0.4
import org.qutim.quickchat 0.4

Item {
    id: root

    property var messages
    property var messageReceipts: { return {}; }
    property QtObject session

    function handleReceipt(id, success) {
        var newMessageReceipts = {};
        for (var i in messageReceipts)
            newMessageReceipts[i] = messageReceipts[i];
        newMessageReceipts[id] = success;

        messageReceipts = newMessageReceipts;
    }
}
