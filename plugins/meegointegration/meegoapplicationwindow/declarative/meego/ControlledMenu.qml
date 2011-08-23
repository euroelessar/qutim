import QtQuick 1.0
import com.nokia.meego 1.0
import org.qutim 0.3

Menu {
	id: root
	property alias controller: menuModel.controller
	MenuLayout {
		Repeater {
			id: repeater
			model: MenuModel {
				id: menuModel
			}

			MenuItem {
				text: action.text
				visible: action.visible && action.enabled
				onClicked: action.trigger()
			}
		}
	}
}
