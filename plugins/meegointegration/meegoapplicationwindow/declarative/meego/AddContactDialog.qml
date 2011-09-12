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
	id: addContactDialog
	AddContactDialogWrapper {
		id: handler
	}

	Connections {
		target: handler
		onShown: {
			addContactDialog.open()
		}
	}

	property Style platformStyle: QueryDialogStyle {}

	title:Text {
		id: textheader
		font.pixelSize: 30
		anchors.centerIn: parent
		color: "white"
		text: qsTr("Add contact");
	}

	content:Column {
		anchors.centerIn: parent
		id: addContactDialogContent
		height: childrenRect.height + addContactDialog.platformStyle.contentTopMargin
		spacing: 10

		Row {
			spacing: 10
			Label {
				id:addContactIdLabel
				color: "white"
				text: handler.contactIdLabel
			}


			TextField {
				anchors.horizontalCenter: parent.horizontalCenter
				id:addContactIdText
				onTextChanged: handler.contactIdText = text
			}
		}

		Row {
			spacing: 10
			Label {
				id:addContactNameLabel
				color: "white"
				text: qsTr("Name")
			}


			TextField {
				anchors.horizontalCenter: parent.horizontalCenter
				id:addContactNameText
				onTextChanged: handler.contactNameText = text
			}
		}
	}

	buttons: Column {
		anchors.top: parent.top
		anchors.centerIn: parent
		spacing: 20

		Button { id:acceptButton; text: qsTr("Add contact"); onClicked: addContactDialog.accept(); platformStyle: ButtonStyle { inverted: true}}
		Button { id:rejectButton; text: qsTr("Cancel"); onClicked: addContactDialog.reject();platformStyle: ButtonStyle { inverted: true}}
	}

	onAccepted: handler.accept()
	onRejected: handler.cancel()
}
