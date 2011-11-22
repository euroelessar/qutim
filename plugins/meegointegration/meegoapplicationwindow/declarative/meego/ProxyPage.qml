import QtQuick 1.0
import com.nokia.meego 1.0
import org.qutim 0.3

Page {
	id: page
	property alias widget: proxy.widget
	signal finished
	WidgetProxy {
		id: proxy
		anchors.fill: parent
	}
	tools: ToolBarLayout {
		id: toolBarLayout
		ToolIcon {
			visible: true
			platformIconId: "toolbar-previous"
			onClicked: {
				finished();
				pageStack.pop();
			}
		}
	}
}
