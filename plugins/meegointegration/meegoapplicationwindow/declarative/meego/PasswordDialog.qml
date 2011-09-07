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
import org.qutim 0.3

Dialog {
	id: passwordDialog
	QuickPasswordDialog {
		id: passwordHandler
	}
	property Style platformStyle: QueryDialogStyle {}

	title:Text {
		id: textheader
		font.pixelSize: 24
		anchors.centerIn: parent
		height: childrenRect.height + 10

		color: "white"
		text: passwordHandler.title
		wrapMode: Text.WordWrap
	}

	content:Column {
		anchors.centerIn: parent
		id: passwordDialogContent
		height: childrenRect.height + passwordDialog.platformStyle.contentTopMargin

		spacing: 10

		TextField {
			anchors.horizontalCenter: parent.horizontalCenter
			id:passwordText
			placeholderText: "Password"
			echoMode: TextInput.PasswordEchoOnEdit
			text: passwordDialog.passwordHandler.passwordText
		}
		Row {
			spacing: 10
			anchors.horizontalCenter: parent.horizontalCenter
			Label {
				id:rememberPasswordText
				anchors.left: parent.left
				anchors.right: rememberPasswordSwitch.anchors.left
				anchors.baseline: rememberPasswordSwitch.anchors.baseline
				color: "white"
				text: qsTr("Remember password")
			}

			Switch {
				id: rememberPasswordSwitch
				checked: passwordDialog.passwordHandler.rememberPassword
			}
		}
	}

	buttons: Column {
		anchors.top: parent.top
		height: childrenRect.height
		anchors.centerIn: parent
		spacing: 10

		Button { id:acceptButton; text: "OK"; onClicked: passwordDialog.accept(); platformStyle: ButtonStyle { inverted: true}}
		Button { id:rejectButton; text: "Cancel"; onClicked: passwordDialog.reject();platformStyle: ButtonStyle { inverted: true}}
	}

	onAccepted: if (passwordDialog.passwordHandler) passwordDialog.passwordHandler.accept()
	onRejected: if (passwordDialog.passwordHandler) passwordDialog.passwordHandler.cancel()
}
