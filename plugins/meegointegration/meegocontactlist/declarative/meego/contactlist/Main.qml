import QtQuick 1.0
import com.nokia.meego 1.0
import org.qutim 0.3

Page { 
	id: root
	tools: ToolBarLayout {
//        ToolItem {
//			iconId: "icon-m-toolbar-back"
//			onClicked: pageStack.pop();
//		}
        ToolItem {
			NotificationBackend {
				id: sound
				type: "Sound"
			}
			iconId: sound.enabled ? "icon-m-toolbar-volume" : "icon-m-toolbar-volume-off"
			onClicked: sound.enabled = !sound.enabled;
		}
//        ToolButton { text: "Two" }
        ToolItem {
			Menu {
		        id: menu
		        visualParent: root.pageStack
		        MenuLayout {
		            MenuItem {text: "Red"; onClicked: { colorRect.color = "darkred" } }
		            MenuItem {text: "Green"; onClicked: { colorRect.color = "darkgreen" }}
		            MenuItem {text: "Blue"; onClicked: { colorRect.color = "darkblue" }}
		        }
		    }
			iconId: "icon-m-toolbar-view-menu"
		}
    }
}
