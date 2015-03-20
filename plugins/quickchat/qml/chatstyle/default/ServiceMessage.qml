import QtQuick 2.3
import org.qutim 0.4
import org.qutim.quickchat 0.4

MessageBase {
    id: root
    height: textItem.y + textItem.height

    readonly property var message: messages[0]
    property alias textColor: textItem.color
    property alias text: textItem.text

    SelectableText {
        id: textItem
        anchors {
            top: parent.top
            left: parent.left
            right: timeItem.left
            margins: 4
        }
        textFormat: Text.RichText
        text: message.formattedHtml()
        color: "gray"
        renderType: Text.NativeRendering
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

        property AnchorsHighlighter highlighter: AnchorsHighlighter {
            Component.onCompleted: {
                // TextEdit.textDocument is non-notifyable property
                textDocument = textItem.textDocument;
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
}
