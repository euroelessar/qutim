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
import "messageDelegate"

Page {
	id: root
	property variant chat
	property variant menu: ControlledMenu {
		controller: chat.activeSession ? chat.activeSession.unit : null
	}
	ListView {
		id: listView
		anchors { top: parent.top; bottom: textField.top }
		width: parent.width
		model: chat.activeSession ? chat.activeSession.model : null
		delegate: MessageDelegate {}
		onCountChanged: {
			positionViewAtIndex(listView.count - 1, ListView.Contain)
		}
	}
	ScrollDecorator {
		flickableItem: listView
	}
//	Connections {
//		target: root.chat
//		onActiveSessionChanged: {
//			if (root.chat.activeSession)
//				tabGroup.currentTab = chatTab
//		}
//	}
	TextArea {
		id: textField
		anchors { left: parent.left; bottom: parent.bottom }
		placeholderText: qsTr("Write anything")
		height: Math.min(200, implicitHeight)
		width: parent.width - sendButton.width
	}
	ToolIcon {
		id: sendButton
		anchors { top: textField.top; right: parent.right; }
		platformIconId: "telephony-sms"
		onClicked: {
			var result = chat.activeSession.send(textField.text)
			if (result != -2)
				textField.text = ""
		}
	}
}
