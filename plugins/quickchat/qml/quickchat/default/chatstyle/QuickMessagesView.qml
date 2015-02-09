import QtQuick 2.3
import QtQuick.Controls 1.2
import org.qutim 0.4
import org.qutim.quickchat 0.4
import ".." as Base

Rectangle {
    id: root
    color: "transparent"

    readonly property QtObject session: chatSession
    property string topic: ""
    property var messages: { return {}; }

    signal appendTextRequested(string text)
    signal appendNickRequested(string nick)

    Connections {
        target: session
        onMessageAppended: root.appendMessage(message)
    }

    Base.ControlledMenu {
        id: menu
    }

    function appendMessage(message) {
        if (message.property("topic", false)) {
            root.topic = message.html;
            return;
        }

        messages[message.id] = message;
        listModel.append({ 'messageId': message.id });
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        frameVisible: false

        ListView {
            id: listView
            model: ListModel {
                id: listModel
            }

            delegate: Item {
                width: listView.width
                height: messageItem.y + messageItem.height

                readonly property var message: root.messages[messageId]
                readonly property bool service: message.property("service", false)
                readonly property bool history: message.property("history", false)
                readonly property bool action: message.action

                Text {
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
//                    visible: !action && !service

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (mouse.button === Qt.LeftButton) {
                                root.appendNickRequested(message.unitData.title);
                            } else if (mouse.button === Qt.RightButton) {
                                var unit = root.session.units.unitById(message.unitData.id);
                                console.log(unit, message.unitData.id);
                                if (unit) {
                                    menu.controller = unit;
                                    menu.popup();
                                }
                            }
                        }
                    }
                }

                Text {
                    id: timeItem
                    anchors {
                        top: parent.top
                        right: parent.right
                        margins: 4
                    }
                    color: "gray"
                    text: message.formatTime('(hh:mm:ss)')
                    renderType: Text.NativeRendering
                }

                Text {
                    id: messageItem
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: nickItem.bottom
                        margins: 4
                    }

                    textFormat: Text.RichText
                    text: message.html
                    renderType: Text.NativeRendering
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }
        }
    }
}
