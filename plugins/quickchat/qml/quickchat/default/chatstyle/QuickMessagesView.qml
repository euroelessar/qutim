import QtQuick 2.3
import org.qutim 0.4
import org.qutim.quickchat 0.4

Rectangle {
    id: root
    color: "transparent"

    readonly property QtObject session: chatSession

    Connections {
        target: session
        onMessageAppended: root.appendMessage(message)
    }

    function appendMessage(message) {
        console.log('new message, text: ', message.text);
        var unitData = message.unitData;
        console.log('tratata');
        console.log('from:', unitData.title);
    }
}
