// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

Page {
	id: root
	anchors.margins: 10
	ServiceManager {
		id: serviceManager
	}
    FlickableColumn {
		spacing: 10
        anchors.fill: parent
		Label {
			text: qsTr("Email:")
		}
		TextField {
			id: jidField
			width: root.width
			inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
			platformSipAttributes: SipAttributes {
				actionKeyLabel: qsTr("Next")
				actionKeyHighlighted: true
			}
			Keys.onReturnPressed: {
				passwordField.focus=true;
			}
		}
		Label {
			text: qsTr("Password:")
		}
		TextField {
			id: passwordField
			width: root.width
			echoMode: TextInput.Password
			inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
			platformSipAttributes: SipAttributes {
				actionKeyLabel: qsTr("Create")
				actionKeyHighlighted: true
			}
			Keys.onReturnPressed: {
				createAccount();
				passwordField.closeSoftwareInputPanel();
			}
		}
	}

	function createAccount()
	{
		var protocols = serviceManager.contactList.protocols;
		for (var i = 0; i < protocols.length; ++i) {
			var protocol = protocols[i];
			if (protocol.id === "mrim") {
				protocol.createAccount(jidField.text, { "password": passwordField.text });
				var page = pageStack.find(function(page) { return page.impl && page.contactList; });
				pageStack.pop(page);
				break;
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
		ToolButton {
			text: qsTr("Create")
			enabled: jidField.text !== ""
			onClicked: {
				createAccount();
			}
		}
	}
}
