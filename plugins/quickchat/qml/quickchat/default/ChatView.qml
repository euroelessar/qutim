import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import org.qutim 0.4

StyledSplitView {
    id: root
    orientation: Qt.Horizontal
    property QtObject session
    readonly property ListModel model: actionsModel

    Config {
        id: config
        group: "unitedWindow/chatView"

        property alias splitViewWidth: splitView.width
        property alias messagesViewHeight: messagesView.height
        property alias chatInputHeight: chatInput.height
        property alias participantsViewWidth: participantsView.width
    }

    Action {
        id: quoteAction
        iconName: "insert-text-quote"
        text: qsTr("Quote")
        tooltip: qsTr("Quote")
    }

    Action {
        id: clearAction
        iconName: "edit-clear-list"
        text: qsTr("Clear chat")
        tooltip: qsTr("Clear chat")
    }

    Action {
        id: separatorAction
        separator: true
    }

    readonly property int actions_count: 3

    ActionContainer {
        id: actionContainer
        visible: root.visible
        actionType: ActionContainer.ChatButton
        controller: root.session ? root.session.unit : null

        onActionAdded: actionsModel.insert(actions_count + index, { modelAction: action })
        onActionRemoved: actionsModel.remove(actions_count + index)
    }

    ListModel {
        id: actionsModel
        Component.onCompleted: {
            append({ modelAction: quoteAction });
            append({ modelAction: clearAction });
            append({ modelAction: separatorAction });
        }
    }

    Connections {
        target: messagesView.item.hasOwnProperty('runJavaScript') ? root.session : null
        onJavaScriptRequest: messagesView.item.runJavaScript(script)
    }

    StyledSplitView {
        id: splitView
        orientation: Qt.Vertical
        width: parent.width - 150
        Layout.fillHeight: true

        Loader {
            id: messagesView
            Layout.fillWidth: true
            height: parent.height - 50
            source: experiment ? "chatstyle/QuickMessagesView.qml" : "ChatMessagesView.qml"
            readonly property bool experiment: false

            readonly property QtObject chatSession: root.session

            Connections {
                target: messagesView.item
                ignoreUnknownSignals: true
                onAppendTextRequested: chatInput.appendText(text)
                onAppendNickRequested: chatInput.appendNick(nick)
            }
        }

        ChatTextArea {
            id: chatInput
            Layout.fillWidth: true
            height: 50
            focus: true
            completionModel: participantsView.model || null
            session: root.session
            frameVisible: false
        }
    }

    ChatParticipantsView {
        id: participantsView
        Layout.fillHeight: true
        width: 150
        model: root.session ? root.session.units : undefined
    }
}
