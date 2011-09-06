/*
 * Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

import QtQuick 1.0
import com.nokia.meego 1.0

Dialog {
	id: passwordDialog
	property variant passwordHandler;

	title:Text {
		id: textheader
		font.pixelSize: 24
		anchors.centerIn: parent

		color: "white"
		text: "Enter password for account %1 (%2)"
		wrapMode: Text.WordWrap
	}

	content:Column {
		id: passwordDialogContent
		anchors.fill: parent
		anchors.margins: 10

		TextField {
			id:passwordText
			placeholderText: "Password"
			echoMode: TextInput.PasswordEchoOnEdit
		}
		CheckBox {
			id:rememberPassword
			text: "Remember password"

		}

	}

	buttons: ButtonRow {
		style: ButtonStyle { }
		anchors.fill: parent

		Button { id:acceptButton; text: "OK"; onClicked: passwordDialog.accept();}
		Button { id:rejectButton; text: "Cancel"; onClicked: passwordDialog.reject();}
	}

	//	content: Item {
	//		property int upperBound: visualParent ? visualParent.height - buttonsRow.height - 64
	//						      : passwordDialog.parent.height - buttonsRow.height - 64
	//		property int __sizeHint: Math.min(Math.max(root.platformStyle.contentFieldMinSize,
	//							   dialogContent.contentHeight),
	//						  upperBound)
	//		height: __sizeHint + passwordDialog.platformStyle.contentTopMargin
	//		width: root.width
	//		DialogContent {
	//			id: dialogContent
	//			invertedTheme: true
	//			handler: root.handler.content
	//		}
	//		Component.onCompleted: {
	//			console.log(__sizeHint, upperBound, passwordDialog.platformStyle.contentTopMargin, dialogContent.contentHeight)
	//		}
	//	}

	//	buttons: Column {
	//		id: buttonsRow
	//		anchors.top: parent.top
	//		width: parent.width
	//		height: childrenRect.height

	//		Repeater {
	//			model: passwordDialog.passwordHandler.buttonNames
	//			Button {
	//				text: modelData
	//				onClicked: {
	//					var names = passwordDialog.passwordHandler.acceptButtons;
	//					for (var i = 0; i < names.length; ++i) {
	//						if (names[i] == text) {
	//							accept();
	//							return;
	//						}
	//					}
	//					reject();
	//				}
	//				platformStyle: ButtonStyle { inverted: true }
	//			}
	//		}
	//	}

	onAccepted: if (passwordDialog.passwordHandler) passwordDialog.passwordHandler.accept()
	onRejected: if (passwordDialog.passwordHandler) passwordDialog.passwordHandler.reject()
}
