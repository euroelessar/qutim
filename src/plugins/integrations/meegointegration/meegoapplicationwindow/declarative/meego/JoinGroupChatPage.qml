// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import org.qutim 0.3
import "constants.js" as UI

Page {
    id: root
    anchors.margins: 10
    property QtObject wrapper
    property variant item

    FlickableColumn {
        anchors.fill: parent
        spacing: 10
        DataForm {
            id: dataForm
            width: parent.width
            item: root.item
        }
        SwitchRow {
            id: saveSwitch
            width: parent.width
            text: qsTr("Save bookmark")
            checked: false
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
            text: qsTr("Join")
            onClicked: {
                if (saveSwitch.checked)
                    root.wrapper.save(dataForm.item, root.item);
                if (root.wrapper.join(dataForm.item)) {
                    var page = pageStack.find(function(page) { return page.applicationWindow && page.contactInfo; });
                    pageStack.pop(page);
                }
            }
        }
	}
}
