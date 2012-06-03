import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0

Page {
	id: root
	property QtObject model
    
    Component {
		id: proxyPageComponent
		ProxyPage {
			id: proxyPage
			onFinished: proxyPage.widget.save()
		}
	}
	
	ListView {
		id: listView
		model: root.model
		anchors.fill: parent
		anchors.leftMargin: 20
		delegate: ItemDelegate {			
			title: model.display
			subtitle: ""
			iconSource: ""
			onClicked: {
                var widget = root.model.widget(index);
                widget.load();
				if (model.isWidget)
					root.pageStack.push(proxyPageComponent, { "widget": widget });
				else
					root.pageStack.push(widget);
			}
			MoreIndicator {
				anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 5 }
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

	Component.onCompleted: console.log("SettingsPage", model)
}
