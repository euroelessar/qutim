import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3

Page {
	id: root
	property variant chat
	tools: ToolBarLayout {
        ToolIcon {
			platformIconId: "icon-m-toolbar-back"
			onClicked: pageStack.pop();
		}
        ToolIcon {
			Menu {
		        id: menu
		        visualParent: root.pageStack
		        MenuLayout {
		            MenuItem {text: "Red"; onClicked: { colorRect.color = "darkred" } }
		            MenuItem {text: "Green"; onClicked: { colorRect.color = "darkgreen" }}
		            MenuItem {text: "Blue"; onClicked: { colorRect.color = "darkblue" }}
		        }
		    }
			platformIconId: "icon-m-toolbar-view-menu"
		}
    }
	ListView {
		id: listView
		width: parent.width
		anchors.top: parent.top
		anchors.bottom: textField.top
		model: chat.activeChannel ? chat.activeChannel.model : null
		delegate: ListDelegate {
		}
	}
	TextField {
		id: textField
		width: parent.width
		anchors.bottom: parent.bottom
		placeholderText: qsTr("Write anything")
	}
}
