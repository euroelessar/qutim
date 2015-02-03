import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import org.qutim 0.4 as Qutim

StyledSplitView {
    id: root
    orientation: Qt.Horizontal
    property alias session: messagesView.session
    readonly property ListModel model: actionsModel

    Qutim.Action {
        id: quoteAction
        iconName: "insert-text-quote"
        text: qsTr("Quote")
        tooltip: qsTr("Quote")
    }

    Qutim.Action {
        id: clearAction
        iconName: "edit-clear-list"
        text: qsTr("Clear chat")
        tooltip: qsTr("Clear chat")
    }

    Qutim.Action {
        id: separatorAction
        separator: true
    }

    readonly property int actions_count: 3

    Qutim.ActionContainer {
        id: actionContainer
        visible: root.visible
        actionType: Qutim.ActionContainer.ChatButton
        controller: session ? session.unit : null

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
        target: root.session
        onJavaScriptRequest: messagesView.runJavaScript(script)
    }

    StyledSplitView {
        orientation: Qt.Vertical
        width: parent.width - 150
        Layout.fillHeight: true

        ChatMessagesView {
            id: messagesView

            Layout.fillWidth: true
            height: parent.height - 50

            onAppendTextRequested: chatInput.appendText(text)
            onAppendNickRequested: chatInput.appendNick(nick)
        }

        ChatTextArea {
            id: chatInput
            Layout.fillWidth: true
            height: 50
            focus: true
            completionModel: participantsView.model || null
            session: messagesView.session
            frameVisible: false
        }
    }

    ChatParticipantsView {
        id: participantsView
        Layout.fillHeight: true
        width: 150
        model: messagesView.session ? messagesView.session.units : undefined
    }
}
