import QtQuick 2.3
import QtQuick.Controls 1.2
import ".." as Base

Rectangle {
    id: root
    color: "transparent"

    readonly property QtObject session: chatSession
    property string topic: ""

    onSessionChanged: session.loadHistory()

    signal appendTextRequested(string text)
    signal appendNickRequested(string nick)

    function copy() {
        layout.copy();
    }

    Connections {
        target: session
        onMessageAppended: root.appendMessage(message)
        onAppendTextRequested: root.appendTextRequested(text)
        onAppendNickRequested: root.appendNickRequested(nick)
    }

    Base.ControlledMenu {
        id: menu
    }

    function appendMessage(message) {
        if (message.property("topic", false)) {
            root.topic = message.html;
            return;
        }

        var keepEnd = flickable.shouldKeepEnd();

        layout.appendMessage(message);

        if (keepEnd)
            flickable.moveToEnd();
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        frameVisible: false

        Flickable {
            id: flickable
            contentWidth: layout.width
            contentHeight: layout.height

            function shouldKeepEnd() {
                return Math.abs((contentY + height - contentHeight) / height) < 0.25;
            }

            function moveToEnd() {
                var newContentY = Math.max(0, contentHeight - height);
                if (contentY !== newContentY)
                    contentY = newContentY;
            }

            onContentHeightChanged: if (shouldKeepEnd()) moveToEnd()
            onContentYChanged: layout.updateHover()

            QuickMessagesLayout {
                id: layout
                width: scrollView.viewport.width
                session: root.session

                messageComponent: Message {}
            }
        }
    }
}
