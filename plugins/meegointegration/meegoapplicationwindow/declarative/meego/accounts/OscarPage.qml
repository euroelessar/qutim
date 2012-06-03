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
    Column {
        spacing: 10
        Label {
            text: qsTr("UIN:")
        }
        TextField {
            id: idField
            inputMethodHints: Qt.ImhDigitsOnly
        }
        Label {
            text: qsTr("Password:")
        }
        TextField {
            id: passwordField
            inputMethodHints: Qt.ImhHiddenText
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
            enabled: idField.text !== ""
            onClicked: {
                var protocols = serviceManager.contactList.protocols;
                for (var i = 0; i < protocols.length; ++i) {
                    var protocol = protocols[i];
                    if (protocol.id === "icq") {
                        protocol.createAccount(idField.text, { "password": passwordField.text });
                        pageStack.pop();
                        pageStack.pop();
                        break;
                    }
                }
            }
        }
	}
}
