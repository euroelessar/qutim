// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import org.qutim 0.3
import "constants.js" as UI

Page {
    id: root
    JoinGroupChatWrapper {
		id: handler
        currentAccount: comboBox.currentAccount
        onAccountsChanged: root.rebuildComboBox()
	}
    function rebuildComboBox() {
        console.debug("rebuildComboBox");
        var currentAccount = comboBox.currentAccount;
        comboBox.model.clear();
        var accounts = handler.accounts;
        console.debug(accounts.length);
        var currentIndex = 0;
        for (var i = 0; i < accounts.length; ++i) {
            var account = accounts[i];
            if (account === currentAccount)
                currentIndex = i;
            comboBox.model.append({
                                      "name": account.id,
                                      "account": account
                                  });
            console.debug(account, account.id);
        }
        comboBox.currentIndex = currentIndex;
    }
    
    function buildSubtitle(map) {
        var list = [];
        for (var key in map) {
            var value = map[key];
            list.push(key + ": <i>" + value + "</i>");
        }
        return list.join("<br />");
    }
    
    Component {
        id: joinComponent
        JoinGroupChatPage {
            wrapper: handler
        }
    }
    Component {
        id: editComponent
        EditGroupChatPage {
            wrapper: handler
        }
    }

    FlickableColumn {
        anchors.fill: parent
        spacing: 10
        ComboBox {
            id: comboBox
            width: parent.width
            title: qsTr("Select account")
            property QtObject currentAccount: currentIndex < 0 ? null : currentItem.account
        }
        GroupSeparator {
            width: parent.width
        }
        BookmarkDelegate {
            title: qsTr("Join")
            subtitle: qsTr("Join groupchat")
            iconSource: "image://theme/icon-m-conversation-group-chat"
            enabled: comboBox.currentAccount !== null
            onClicked: pageStack.push(joinComponent, { item: handler.fields() })
        }
        BookmarkDelegate {
            title: qsTr("Edit bookmarks")
            subtitle: qsTr("Manage bookmarks")
            iconSource: "image://theme/icon-m-content-settings"
            enabled: comboBox.currentAccount !== null
            onClicked: pageStack.push(editComponent)
        }
        GroupSeparator {
            width: parent.width
            text: qsTr("Bookmarks")
        }
        Repeater {
            model: handler.bookmarks
            delegate: BookmarkDelegate {
                title: modelData.title
                subtitle: root.buildSubtitle(modelData.fields)
                iconSource: "image://theme/icon-m-content-favourites"
                onClicked: {
                    handler.join(modelData.item);
                    pageStack.pop();
                }
            }
        }
        GroupSeparator {
            width: parent.width
            text: qsTr("Recent")
        }
        Repeater {
            model: handler.recent
            delegate: BookmarkDelegate {
                title: modelData.title
                subtitle: root.buildSubtitle(modelData.fields)
                iconSource: "image://theme/icon-m-common-clock"
                onClicked: {
                    handler.join(modelData.item);
                    pageStack.pop();
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
    Component.onCompleted: root.rebuildComboBox()
}
