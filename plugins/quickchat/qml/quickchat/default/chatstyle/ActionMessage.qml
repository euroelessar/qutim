import QtQuick 2.3
import org.qutim 0.4
import org.qutim.quickchat 0.4

ServiceMessage {
    id: root

    readonly property string nick: message.unitData.title
    readonly property url appendNickUrl: session.appendNickUrl(nick) + "#do-not-style"
    readonly property string nickHtml: {
        return "<b><a href=\"" + appendNickUrl + "\"" +
                " style=\"text-decoration: none; color:" + textColor + ";\">" +
                session.htmlEscape(nick) + "</a></b>";
    }
    readonly property string formattedHtml: message.formattedHtml().substr(4)
    textColor: message.incoming ? "#ff6600" : "#0078ff"
    text: nickHtml + " " + formattedHtml
}
