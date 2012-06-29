import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3

Page {
    id: root
    anchors.margins: 10
    property QtObject contactInfo
    
    Flickable {
        id: flickable
		anchors.fill: parent
        contentWidth: dataForm.width
        contentHeight: dataForm.height
            
        DataForm {
            id: dataForm
            width: flickable.width
            item: root.contactInfo.item
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
			visible: true
            text: qsTr("Request")
            onClicked: root.contactInfo.request()
		}
        ToolButton {
			visible: !root.contactInfo.readOnly
            text: qsTr("Save")
            onClicked: root.contactInfo.save(dataForm.item)
		}
	}
}
