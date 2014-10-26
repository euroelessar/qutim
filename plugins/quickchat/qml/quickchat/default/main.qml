import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import org.qutim 0.4
import org.qutim.quickchat 0.4
import 'logic.js' as Logic

ApplicationWindow {
    id: window

    height: 600
    width: 800
    visible: true
    title: "Chat Window"

    function updateToolBar() {
        var tab = tabs.getTab(tabs.currentIndex);
        var model = tab && tab.item && tab.item.model;
        console.log(tabs.currentIndex, tab, tab && tab.item, model);
        toolBarRepeater.model = model;
    }

    Component {
        id: toolButtonComponent

        ToolButton {
            id: button
            checkable: modelAction.checkable
            checked: modelAction.checked
            iconName: modelAction.iconName
            text: modelAction.text
            tooltip: modelAction.tooltip
            Connections {
                target: modelAction
                onCheckedChanged: button.checked = modelAction.checked
            }
            onClicked: modelAction.isCheckable ? modelAction.trigger() : modelAction.trigger()
        }
    }

    toolBar: ToolBar {
        id: toolBar

        RowLayout {
            anchors.fill: parent

            ToolButton {
                anchors.verticalCenter: parent.verticalCenter
                iconName: "insert-text-quote"
                text: qsTr("Quote")
                tooltip: qsTr("Quote")
            }

            ToolButton {
                iconName: "edit-clear-list"
                text: qsTr("Clear chat")
                tooltip: qsTr("Clear chat")
            }

            RowLayout {
                Repeater {
                    id: toolBarRepeater
                    delegate: toolButtonComponent
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "#ff0000"
            }
        }
    }

    Service {
        id: chatLayer
        name: "ChatLayer"
    }

    Connections {
        target: chatLayer.object

        onSessionCreated: {
            Logic.addSession(session);
        }

        onSessionDestroyed: {
            Logic.removeSession(session);
        }

        onShown: {
            window.show();
            window.requestActivate();
        }

        onActiveSessionChanged: {
            Logic.setActiveSession(session);
        }
    }

    TabView {
        id: tabs
        anchors.fill: parent
        tabPosition: Qt.BottomEdge
        frameVisible: false

        onCurrentIndexChanged: {
            var tab = tabs.getTab(currentIndex);
            var session = tab && tab.item.session;
            chatLayer.object.activeSession = session;
            window.updateToolBar();
        }

        Component {
            id: tabComponent

            ChatView {
            }
        }
    }

    Component.onCompleted: {
        chatLayer.object.forEachChannel(Logic.addSession);
    }
}
