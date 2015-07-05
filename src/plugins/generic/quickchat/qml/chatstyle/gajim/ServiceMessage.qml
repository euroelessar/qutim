import QtQuick 2.3
import org.qutim 0.4
import org.qutim.quickchat 0.4
import "../default" as Default

MessageBase {
    id: root
    height: messageItem.height

    readonly property var message: messages[0]

    Default.SelectableText {
        id: messageItem
        anchors {
            top: parent.top
            left: parent.left
            margins: 4
        }
        renderType: Text.NativeRendering
        textFormat: Text.RichText
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere



        readonly property string timeText: Qt.formatDateTime(message.time, '[hh:mm:ss]')
        readonly property string timeColor: {
            var incoming = message.incoming;
            var success = messageReceipts[message.id];
            var history = message.property("history", false);
            var color = undefined;
            if (history)
                color = "gray";
            else if (incoming || success === true)
                color = "black";
            else if (success === false)
                color = "red";
            else
                color = "orange";
            return color;
        }

        readonly property string timeHtml: "<span style=\"color:" + timeColor + ";\">" + timeText + "</span>";


        property variant nickColorsArray: ["#4e9a06", "#f57900", "#ce5c00", "#3465a4", "#204a87", "#75507b", "#5c3566", "#c17d11", "#8f5902", "#ef2929", "#cc0000", "#a40000"]
        readonly property string incomingNickColor: {
            return nickColorsArray[root.session.hashOf(nick, nickColorsArray.length)];
        }
        readonly property string nick: message.unitData.title
        readonly property url appendNickUrl: session.appendNickUrl(nick) + "#do-not-style"
        readonly property string nickHtml: {
            return "<b><a href=\"" + appendNickUrl + "\"" +
                    " style=\"text-decoration: none; color:" + nickColor + "; font-weight:" + fontWeight + ";\">" +
                    session.htmlEscape(nick) + "</a></b>";
        }

        readonly property string fontWeight: {
            return message.property("mention", false) ? "bold" : "normal";
        }

        readonly property string nickColor: message.incoming ? incomingNickColor : "#437ee0"

        readonly property string messageFormattedHtml: "<span style=\"color:" + messageColor + "; font-weight: " + fontWeight + ";\" >" + message.formattedHtml() + "</span>";
        readonly property string messageColor: {
            return "#509d1f"
        }

        text: timeHtml + " " + messageFormattedHtml

        property AnchorsHighlighter highlighter: AnchorsHighlighter {
            Component.onCompleted: {
                // TextEdit.textDocument is non-notifyable property
                textDocument = messageItem.textDocument;
                hoverColor = "#07A";
                activeColor = "#09C";
            }
        }
    }


}
