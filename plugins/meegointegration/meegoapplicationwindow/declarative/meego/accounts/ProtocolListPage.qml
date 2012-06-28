// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import ".."

Page {
    id: root
    anchors.margins: 10
    ListView {
        anchors.fill: parent
        model: ListModel {
            id: listModel
        }
        delegate: ListDelegate {
            onClicked: pageStack.push(Qt.createComponent(model.pageSource))
            MoreIndicator {
				anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 5
                }
			}
        }
    }
    tools: ToolBarLayout {
		id: toolBarLayout
		ToolIcon {
			visible: true
			platformIconId: "toolbar-previous"
			onClicked: pageStack.pop()
		}
	}
    Component.onCompleted: {
        var protocols = [
                    {
                        title: "Jabber",
                        subtitle: qsTr("Xmpp, Ya.Online, GTalk, LiveJournal"),
                        pageSource: "CreateJabberPage.qml"
                    },
                    {
                        title: "ICQ",
                        subtitle: "Oscar",
                        pageSource: "CreateOscarPage.qml"
                    },
                    {
                        title: qsTr("VKontakte"),
                        subtitle: qsTr("VKontakte social network"),
                        pageSource: "CreateVKPage.qml"
                    },
                    {
                        title: "Mail.ru",
                        subtitle: qsTr("Mail.ru Instant Messenger"),
                        pageSource: "CreateMrimPage.qml"
                    }
                ];
        for (var i = 0; i < protocols.length; ++i)
            listModel.append(protocols[i]);
    }
}
