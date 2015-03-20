import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import org.qutim 0.4 as Qutim
import org.qutim.quickchat 0.4
import 'logic.js' as Logic

BaseWindow {
    id: window

    height: 600
    width: 800
    visible: true
    title: "qutIM"

    Qutim.Config {
        id: config
        group: "unitedWindow/mainWindow"

        property alias height: window.height
        property alias width: window.width
        property alias x: window.x
        property alias y: window.y
        property alias contactsViewWidth: contactsView.width
        property alias tabViewWidth: tabs.width
    }

    function updateToolBar() {
        var tab = tabs.getTab(tabs.currentIndex);
        var model = tab && tab.item && tab.item.model;
        console.log(tabs.currentIndex, tab, tab && tab.item, model);
        window.toolBarModel = model;
    }

    Qutim.Service {
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

    StyledSplitView {
        anchors.fill: parent

        ContactsView {
            id: contactsView
            Layout.fillHeight: true
            width: 50
            searchText: window.searchText
        }

        TabView {
            id: tabs
            tabPosition: Qt.BottomEdge
            frameVisible: false
            Layout.fillHeight: true
            width: parent.width - contactsView.width

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
    }

    Component.onCompleted: {
        chatLayer.object.forEachChannel(Logic.addSession);
    }
}
