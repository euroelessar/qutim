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
	id: authDialog
	AuthDialogWrapper {
		id: handler
	}

	Connections {
		target: handler
		onShown: {
			authDialog.open()
		}
	}

	property Style platformStyle: QueryDialogStyle {}

	title:Text {
		anchors.right: parent.right
		anchors.left: parent.left
		id: textheader
		font.pixelSize: 30
		color: "white"
		text: handler.title
		wrapMode: Text.WordWrap
	}


	content:TextArea {
		anchors.horizontalCenter: parent.horizontalCenter
		id:messageText
		width:parent.width
		height: Math.max (200, implicitHeight)
		onTextChanged: handler.messageText = text
		text:handler.messageText
		readOnly: handler.isIncoming
	}

	buttons: Column {
		anchors.centerIn: parent
		spacing: 20

		Button { id:acceptButton; text: qsTr("Accept"); onClicked: authDialog.accept(); platformStyle: ButtonStyle { inverted: true}}
		Button { id:rejectButton; text: qsTr("Reject"); onClicked: authDialog.reject();platformStyle: ButtonStyle { inverted: true}}
	}

	onAccepted: handler.accept()
	onRejected: handler.cancel()
}
