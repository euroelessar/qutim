import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import org.qutim 0.4

SplitView {
    id: root
    orientation: Qt.Horizontal
    property alias session: messagesView.session
    readonly property ListModel model: actionsModel

    ActionContainer {
        id: actionContainer
        visible: root.visible
        actionType: ActionContainer.ChatButton
        controller: session ? session.unit : null

        onActionAdded: {
            console.log('action added', index, action.iconName, action.text, action)
            actionsModel.insert(index, { modelAction: action })
        }
        onActionRemoved: actionsModel.remove(index)
    }

    ListModel {
        id: actionsModel
    }

    Connections {
        target: root.session
        onJavaScriptRequest: messagesView.runJavaScript(script)
    }

    SplitView {
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
        }
    }

    ChatParticipantsView {
        id: participantsView
        Layout.fillHeight: true
        width: 150
        model: messagesView.session ? messagesView.session.units : undefined
    }
}
