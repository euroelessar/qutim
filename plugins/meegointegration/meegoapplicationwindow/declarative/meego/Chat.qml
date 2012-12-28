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

import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3

Page {
	id: root
	property variant chat
	property variant menu: contactMenu

	ControlledMenu {
		id: contactMenu
		visualParent: pageStack
		controller: chat.activeSession ? chat.activeSession.unit : null
	}

	property variant currentSessionPage

	Connections {
		target: root.chat
		onActiveSessionChanged: {
			root.currentSessionPage = session.page;
		}
		onSessionCreated: {
			if (!session.page)
				session.page = webViewComponent.createObject(root, { "session": session });
			root.currentSessionPage = session.page;
		}
		onSessionDestroyed: {
			if (session.page) {
				var page = session.page;
				session.page = null;
				page.destroy();
			}
		}
	}

	Component {
		id: webViewComponent
		ChatView {
			id: chatView
			anchors{ top:parent.top; left:parent.left; right:parent.right; bottom: textField.top}
			//visible: chatView.session.active
			visible: chatView === root.currentSessionPage
		}
	}

	EmoticonsDialog {
		id:emoticonsDialog
		anchors{ top:parent.top; left:parent.left; right:parent.right; bottom: textField.top}
		onStateChanged: {
			if (emoticonsDialog.state === "hidden" && emoticonsDialog.selectedEmoticon !== undefined)
			{
				var currentPosition = textField.cursorPosition;
				textField.text = [textField.text.toString().slice(0, textField.cursorPosition),
						  emoticonsDialog.selectedEmoticon, textField.text.toString().slice(textField.cursorPosition)].join('');
				textField.cursorPosition = currentPosition + emoticonsDialog.selectedEmoticon.length
			}
			textField.focus = true
		}
	}

	Emoticons{
		id:emoticons
	}

	ToolButton {
		id: emoticonsButton
		width: visible ? 50 : 0
		height: 50
		anchors { top: textField.top; left: parent.left; topMargin:5; leftMargin:5; bottomMargin: 5 }
		iconSource: "image://theme/icon-m-messaging-smiley-happy"
		onClicked: {
			if (emoticonsDialog.state === "hidden")
				emoticonsDialog.show();
			else
				emoticonsDialog.hide();
		}
		visible: emoticons.isCurrentEmoticonsAvailable
	}

	TextArea {
		id: textField
		anchors { left: emoticonsButton.right; bottom: parent.bottom; right: sendButton.left; leftMargin:5; rightMargin:5}
		placeholderText: qsTr("Write anything")
		height: Math.min(200, implicitHeight)
	}

	ToolButton {
		id: sendButton
		width: 50
		height: 50
		anchors { top: textField.top; right: parent.right; topMargin:5; rightMargin:5}
		iconSource: "image://theme/icon-m-toolbar-send-chat"
		onClicked: {
			if (textField.text!=="")
			{
				var result = chat.activeSession.send(textField.text);
				if (result !== -2)
					textField.text = "";
			}
		}
	}
}
