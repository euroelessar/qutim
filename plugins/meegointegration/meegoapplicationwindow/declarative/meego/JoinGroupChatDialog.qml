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
	id: joinGroupChatDialog
	JoinGroupChatWrapper {
		id: handler
	}
	Connections {
		target: handler
		onJoined: joinGroupChatDialog.accept();
		onJoinDialogShown:pageStack.push(joinPage);
		onBookmarksEditDialogShown: {}
		onShown: joinGroupChatDialog.open();

	}

	acceptButtonText: qsTr("Join")
	rejectButtonText: qsTr("Close")

	content: Item {
		id: contentArea
		objectName: "contentArea"
		anchors.fill: parent
		anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
		anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0

		PageStack {
			id: pageStack
			anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
			anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0
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

		Column {
			anchors.fill: parent
			id: dialogContent
			spacing: 10
			Text {
				id:header
				anchors{ left:parent.left; right:parent.right}
				font.pointSize: 20
				text:qsTr("Select account:")
			}
			Row {
				id: accounts
				anchors{ left:parent.left; right:parent.right}

				Button {
					id: combobox
					anchors {
						margins: 10
						left: parent.left
						right: parent.right
					}
					text: handler.accounts.currentIndex
					onClicked: accountsDialog.open()
				}

				Image {
					id: image
					anchors { right: combobox.right; verticalCenter: combobox.verticalCenter }
					property variant __pressed: combobox.pressed ? "-pressed" : ""
					source: "image://theme/meegotouch-combobox-indicator" + __pressed
					MouseArea {
						onClicked: accountsDialog.open()
					}
				}

				SelectionDialog {
					id: accountsDialog
					titleText: qsTr("Select account:")
					model: handler.accounts
					//				delegate: ItemDelegate {
					//					title: account.id
					//					subtitle: account.name
					//				}
					onAccepted: {
						handler.onAccountBoxActivated(accountsDialog.selectedIndex);
						accountsDialog.model.clear();
					}
					onRejected: accountsDialog.model.clear()

				}

			}

			ListView {
				id:bookmarks
				spacing: 20
				anchors{ left:parent.left; right:parent.right;}

				model: handler.bookmarks

				onCurrentIndexChanged:
				{
					handler.onItemActivated(bookmarks.currentItem)
				}
				delegate: ItemDelegate {
					title: display
					subtitle: description
				}


				//			delegate: ItemDelegate {
				//				title: account.id
				//				subtitle: account.name
				//				onClicked: handler.setAccount(account.id);
				//			}
			}
		}


	}

	Page
	{
		id:joinPage
		Column {
			anchors.fill: parent
			id: joinDialog
			spacing: 10
			Text {
				id:joinHeader
				anchors.horizontalCenter: parent.horizontalCenter;
				font.pointSize: 30
				text:qsTr("Join to conference:")
			}


			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.right: parent.right
				spacing: 10
				Label {
					id:conferenceNameLabel
					anchors.right: parent.horizontalCenter
					anchors.rightMargin: 10
					text: qsTr("name")
				}
				TextField {
					anchors.left: parent.horizontalCenter
					anchors.right: parent.right
					id:conferenceNameText
				}
			}

			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.right: parent.right
				spacing: 10
				Label {
					id:conferenceLabel
					anchors.right: parent.horizontalCenter
					anchors.rightMargin: 10

					text: qsTr("Conference")
				}
				TextField {
					anchors.left: parent.horizontalCenter
					anchors.right: parent.right
					id:conferenceText
				}
			}

			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.right: parent.right
				spacing: 10
				Label {
					id:nickLabel
					anchors.right: parent.horizontalCenter
					anchors.rightMargin: 10

					text: qsTr("Nick")
				}
				TextField {
					anchors.left: parent.horizontalCenter
					anchors.right: parent.right
					id:nickText
				}
			}

			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.right: parent.right
				spacing: 10
				Label {
					id:passwordLabel
					anchors.right: parent.horizontalCenter
					anchors.rightMargin: 10

					text: qsTr("Password")
				}
				TextField {
					anchors.left: parent.horizontalCenter
					anchors.right: parent.right
					id:passwordText
				}
			}


			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.right: parent.right
				spacing: 10
				Label {
					id:autoJoinLabel
					anchors.right: parent.horizontalCenter
					anchors.rightMargin: 10

					text: qsTr("Auto Join")
				}
				Switch {
					anchors.left: parent.horizontalCenter
					anchors.right: parent.right
					id:autoJoinSwitch
				}
			}

			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.right: parent.right
				spacing: 10
				Label {
					id:saveBookmarkLabel
					anchors.right: parent.horizontalCenter
					anchors.rightMargin: 10

					text: qsTr("Save to bookmark")
				}
				Switch {
					anchors.left: parent.horizontalCenter
					anchors.right: parent.right
					id:saveBookmarkSwitch
				}
			}

			Button {
				id:acceptButton;
				anchors.horizontalCenter: parent.horizontalCenter;
				text: qsTr("Join");
				onClicked: {
					handler.join()

				}
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
		if (status == PageStatus.Inactive && pageStack.currentPage != mainPage) {
			pageStack.clear();
			pageStack.push(mainPage);
		}

	}
	Component.onCompleted: {
		__owner = parent;
		pageStack.push(mainPage);
	}
}
