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
            ListElement {
                title: "Jabber"
                subtitle: "Xmpp, Ya.Online, GTalk, LiveJournal"
                pageSource: "JabberPage.qml"
            }
            ListElement {
                title: "ICQ"
                subtitle: "Oscar"
                pageSource: "OscarPage.qml"
            }
        }
        delegate: ListDelegate {
            onClicked: {
                pageLoader.source = model.pageSource
                pageStack.push(pageLoader.item)
            }
            MoreIndicator {
				anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 5
                }
			}
        }
        Loader {
            id: pageLoader
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
}
