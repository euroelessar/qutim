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

	BookmarksModel
	{
		id:bookmarksModel
	}

	Connections {
		target: handler
		onJoined: joinGroupChatDialog.accept();
		onJoinDialogShown:pageStack.push(joinPage);
		onCurrentAccountIdChanged: { bookmarksModel.fill(handler.currentAccountId)}
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

	SelectionDialog {
		id: accountsDialog
		titleText: qsTr("Select account:")

		model: ListModel { }

		onSelectedIndexChanged:
		{
			console.log("Current join account changed");
			if (accountsDialog.selectedIndex>=0)
			handler.setAccount(accountsDialog.selectedIndex);
		}
		onAccepted: accountsDialog.model.clear();
		onRejected: accountsDialog.model.clear();


		function chooseValue() {
			console.log("Choose join account");
			var values = handler.accountIds();
			if (values.length===1){
				accountsDialog.selectedIndex = 0;
				handler.setAccount(accountsDialog.selectedIndex);
				return;
			}

			for (var i = 0; i < values.length; ++i) {
				accountsDialog.model.append({ "name": values[i] });
			}
			accountsDialog.open();
		}
	}

	Component {
		id: mainPageComponent
		Page {
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
						text: handler.currentAccountId
						onClicked: accountsDialog.chooseValue()
					}
	
					Image {
						id: image
						anchors { right: combobox.right; verticalCenter: combobox.verticalCenter }
						property variant __pressed: combobox.pressed ? "-pressed" : ""
						source: "image://theme/meegotouch-combobox-indicator" + __pressed
						MouseArea {
							onClicked: accountsDialog.chooseValue()
						}
					}
	

				}
	
				ListView {
					id:bookmarks
					spacing: 20
					anchors{ left:parent.left; right:parent.right; top:accounts.bottom;bottom:parent.bottom}
					clip:true
					model: bookmarksModel
	
					delegate:Column {
						id: wgt
						anchors{ left:parent.left; right:parent.right;}
						property alias bookmarkItem_visible: bookmarkItem.visible
						property alias separatorItem_visible: separatorItem.visible
	
						state: bookmarkType == 3 ? "s1" : "s2"
	
						ItemDelegate {
							id:bookmarkItem
							title: name
							subtitle: description
							onClicked: {
								console.log(separator)
								console.log(bookmarkType)
								switch(bookmarkType)
								{
								case 2:
								case 4:
									handler.join(bookmarkData);
									break;
								case 0:
									pageStack.push(joinPageComponent)
									break;
								case 1:
									break;
								}
							}
	
							Rectangle {
								id: separatorItem
								height: 20
							}
	
							states: [
								State {
									name: "s1"
									PropertyChanges {
										target: wgt
										bookmarkItem_visible: true
										separatorItem_visible: false
									}
								},
								State {
									name: "s2"
									PropertyChanges {
										target: wgt
										bookmarkItem_visible: false
										separatorItem_visible: true
									}
								}
	
							]
						}
	
	
					}
				}
			}
		}
	}
	Component {
		id: joinPageComponent
		Page {
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
						text: qsTr("Name")
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
	}
	Component.onCompleted: {
		pageStack.push(mainPageComponent);
		var values = handler.accountIds();
		if (values.length===1){
			accountsDialog.selectedIndex=0;
			handler.setAccount(accountsDialog.selectedIndex);
		}

	}
}
