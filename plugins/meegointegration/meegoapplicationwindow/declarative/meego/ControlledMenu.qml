/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3

Menu {
	id: root
	property alias controller: menuModel.controller
	property variant __model : VisualDataModel {
		id: visualModel
        model: MenuModel {
			id: menuModel
		}
        delegate: MenuItem {
			text: action.text
			visible: action.visible && action.enabled
			onClicked: {
				console.log("ControllerMenu", action.text, model.hasModelChildren)
				if (model.hasModelChildren) {
					visualModel.rootIndex = visualModel.modelIndex(index)
				} else {
					action.trigger()
					root.close()
				}
			}
			MoreIndicator {
				anchors { verticalCenter: parent.verticalCenter; right: parent.right }
				visible: model.hasModelChildren
			}
		}
    }
	Column {
		id: menuLayout
		
		anchors.left: parent.left
		anchors.right: parent.right
		height: childrenRect.height
		
		Repeater {
			id: repeater
			model: visualModel
		}
		
		function closeLayout() {
			// Do nothing
		}
	}
}
