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

Sheet {
	id: addContactDialog
	AddContactDialogWrapper {
		id: handler
		onShowAccountsListChanged: {
			if (showAccountsList == false)
				pageStack.push(addContactPage);
			else {pageStack.clear();
				pageStack.push(mainPage);}
		}
	}

	acceptButtonText: qsTr("Close")

	content: Item {
		id: contentArea
		objectName: "contentArea"
		anchors.fill: parent
		anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
		anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0

		PageStack {
			id: pageStack
			anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
			//anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0
			toolBar: toolBar
		}
		ToolBar {
			objectName: "toolBar"
			anchors.top: contentArea.bottom
			id: toolBar
			// Don't know why I have to do it manually
			onVisibleChanged: if (__currentContainer) __currentContainer.visible = visible;
		}
	}
	Page
	{
		id:mainPage
		anchors.margins:10
		anchors.fill: parent

		Text {
			id:header
			anchors{top:parent.top; left:parent.left; right:parent.right}
			font.pointSize: 35
			text:qsTr("Select protocol:")
		}

		ListView {
			id:mainInfo
			spacing: 20
			anchors{top:header.bottom; left:parent.left; right:parent.right;bottom:parent.bottom}
			clip:true
			model: handler
			delegate: ItemDelegate {
				title: account.id
				subtitle: account.name
				enabled: isEnabled
				onClicked: handler.setAccount(account.id);
			}
		}
	}

	Page
	{
		id:addContactPage

		Text {
			id:addContactHeader
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.top: parent.top
			font.pointSize: 30
			text:qsTr("Add contact for")
		}

		Text {
			id:addContactId
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.top: addContactHeader.bottom
			anchors.topMargin: 20
			font.pointSize: 30
			text: handler.contactIdLabel
		}

		Label {
			id:addContactIdLabel
			anchors{right: parent.horizontalCenter; rightMargin: 10; top: addContactId.bottom; topMargin: 20}
			text: qsTr("Contact ID")
		}
		TextField {
			id:addContactIdText
			anchors{left: parent.horizontalCenter; leftMargin: 10; right: parent.right; top: addContactId.bottom; topMargin: 20}
		}

		Label {
			id:addContactNameLabel
			anchors{right: parent.horizontalCenter; rightMargin: 10; top: addContactIdText.bottom; topMargin: 20}
			text: qsTr("Name for contact")
		}
		TextField {
			id:addContactNameText
			anchors{left: parent.horizontalCenter; leftMargin: 10; right: parent.right; top: addContactIdText.bottom; topMargin: 20}
		}

		Button {
			id:acceptButton;
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.top: addContactNameText.bottom;
			anchors.topMargin: 20
			text: qsTr("Add contact");
			onClicked: {
				handler.addContact(addContactIdText.text,addContactNameText.text);
				addContactDialog.accept();
			}
		}

		tools: ToolBarLayout {
			ToolIcon {
				visible: true
				platformIconId: "toolbar-previous"
				onClicked: pageStack.pop()
			}
		}

	}

	onStatusChanged: {
		if (status === PageStatus.Inactive && pageStack.currentPage !== mainPage) {
			pageStack.clear();
			pageStack.push(mainPage);
		}
		if (status === DialogStatus.Opening)
			handler.loadAccounts();
	}
	Component.onCompleted: {
		pageStack.push(mainPage);
	}
}
