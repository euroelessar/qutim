import QtQuick 2.3
import org.qutim 0.4
import org.qutim.quickchat 0.4

Item {
    id: root
    width: listView.width
    height: column.y + column.height

    property var messages
    property var messageReceipts: { return {}; }
    property QtObject session
    readonly property var message: messages[0]
    readonly property bool history: message.property("history", false)

    function handleReceipt(id, success) {
        var newMessageReceipts = {};
        for (var i in messageReceipts)
            newMessageReceipts[i] = messageReceipts[i];
        newMessageReceipts[id] = success;

        messageReceipts = newMessageReceipts;
    }

    SelectableText {
        id: nickItem
        anchors {
            top: parent.top
            left: parent.left
            margins: 4
        }
        textFormat: Text.PlainText
        font.bold: true
        text: message.unitData.title
        color: message.incoming ? "#ff6600" : "#0078ff"
        renderType: Text.NativeRendering

        function click(x, y) {
            root.session.appendNick(message.unitData.title);
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                var unit = root.session.units.unitById(message.unitData.id);
                console.log(unit, message.unitData.id);
                if (unit) {
                    menu.controller = unit;
                    menu.popup();
                }
            }
        }
    }

    SelectableText {
        id: timeItem
        anchors {
            top: parent.top
            right: parent.right
            margins: 4
        }
        color: "gray"
        text: Qt.formatDateTime(message.time, '(hh:mm:ss)')
        renderType: Text.NativeRendering
    }

    Column {
        id: column
        anchors {
            left: parent.left
            right: parent.right
            top: nickItem.bottom
            margins: 4
        }
        Repeater {
            model: root.messages.length

            SelectableText {
                id: messageItem
                width: column.width
                textFormat: Text.RichText
                text: bulletHtml + formattedHtml
                renderType: Text.NativeRendering
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                readonly property var message: root.messages[index]
                readonly property string bulletHtml: {
                    var incoming = message.incoming;
                    var success = messageReceipts[message.id];
                    var history = message.property("history", false);
                    var color = undefined;
                    if (history)
                        color = "gray";
                    else if (incoming || success === true)
                        color = "green";
                    else if (success === false)
                        color = "red";
                    else
                        color = "orange";
                    return "<span style=\"color:" + color + "\">●</span>&nbsp;";
                }
                readonly property string formattedHtml: message.formattedHtml()
                property AnchorsHighlighter highlighter: AnchorsHighlighter {
                    Component.onCompleted: {
                        // TextEdit.textDocument is non-notifyable property
                        textDocument = messageItem.textDocument;
                    }
                }
            }
        }
    }
}
