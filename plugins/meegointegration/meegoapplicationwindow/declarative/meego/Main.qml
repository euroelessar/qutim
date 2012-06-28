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
import org.qutim 0.3
import "accounts"

PageStackWindow {
	id: root
	ServiceManager {
		id: serviceManager
	}
	property variant contactList:  serviceManager.contactList
	property variant chat:  serviceManager.chatLayer
	property variant settings:  serviceManager.settingsLayer
    property variant vibration:  serviceManager.vibration
	Connections {
		target: root.chat
		onShown: {
			if (root.chat.activeSession)
				tabGroup.currentTab = chatTab
		}
	}
    Connections {
		target: root.contactList
        onStarted: {
            if (contactList.accounts.length === 0) {
                pageStack.push(accountCreatorPageComponent);
            }
        }
    }
    Connections {
		target: root.settings
        onSettingsRequested: pageStack.push(settingsPageComponent, { "model": model })
	}
	Connections {
		target: application
		onWidgetShown: root.pageStack.push(proxyPageComponent, { "widget": widget })
		onWidgetClosed: root.pageStack.pop(proxyPageComponent)
	}
	Component {
		id: proxyPageComponent
		ProxyPage {
		}
	}
    Component {
		id: accountCreatorPageComponent
		ProtocolListPage {
		}
	}
	Statistics {
		id: statistics
	}

	PasswordDialog{
		id:passwordDialog
	}
	SettingsDialog {
		id:settingsDialog
	}
	AuthDialog {
		id:authDialog
	}
	JoinGroupChatDialog {
		id:joinGroupChatDialog
	}
	AboutDialog {
		id:aboutDialog
	}
	AddContactDialog {
		id:addContactDialog
	}
	Notifications {
		id:notifications
		windowActive: platformWindow.active
        onWindowActiveChanged: {
            root.vibration.windowActive = windowActive
            var session = root.chat.activeSession;
            if (session !== undefined && session !== null && tabGroup.currentTab === chatTab) {
                if (windowActive)
                    session.active = true;
                else
                    session.active = false;
            }
        }
	}

	initialPage: Page {
		AnimatedTabGroup {
			id: tabGroup
			anchors.fill: parent

			currentTab: contactListTab
			
			ContactList {
				id: contactListTab
				contactList: root.contactList
				chat: root.chat

			}

			ChatChannelList {
				id: channelListTab
				chat: root.chat
			}
			Chat {
				id: chatTab
				chat: root.chat
			}
			ChatUnitsList {
				id: conferenceUsersTab
				chat: root.chat
			}
            
            onCurrentTabChanged: {
                var session = root.chat.activeSession;
                if (session !== undefined && session !== null) {
                    if (currentTab !== chatTab) {
                        session.active = false;
                    } else {
                        session.active = true;
                    }
                }
            }
		}
		tools: ToolBarLayout {
			ButtonRow {
				style: TabButtonStyle { }
				TabIcon {
					platformIconId: "toolbar-contact"
					tab: contactListTab
				}
				TabIcon {
					platformIconId: "toolbar-list"
					tab: channelListTab
				}
				TabIcon {
					platformIconId: "toolbar-new-chat"
					tab: chatTab
					enabled: chat.activeSession !== null
				}
				TabIcon {
					platformIconId: "toolbar-new-chat"
					tab: conferenceUsersTab
					enabled: chat.activeSession !== null && chat.activeSession.unit.conference
				}
			}

			ToolIcon {
				property variant menu: tabGroup.currentTab.menu
				//				visible: menu !== undefined
				//				Menu {
				//					id: menu
				//					visualParent: root.pageStack
				//				}
				platformIconId: "toolbar-view-menu"
				onClicked:mainMenu.open()
//				onClicked: (menu.status == DialogStatus.Closed)
//					   ? menu.open()
//					   : menu.close()
			}

		}
		Menu {
		    id: mainMenu

		    content: MenuLayout {
			MenuItem {
				text: qsTr("Show/hide offline contacts")
			    onClicked: contactListTab.showOffline=!contactListTab.showOffline;
			}
			MenuItem {
				text: qsTr("Join group chat")
			    onClicked: joinGroupChatDialog.open();
			}
			MenuItem {
				text: qsTr("About qutIM")
			    onClicked: aboutDialog.open();
			}
			MenuItem {
				text: qsTr("Add contact")
			    onClicked: addContactDialog.open();
			}
			MenuItem {
				text: qsTr("Settings")
			    onClicked: root.settings.show();
			}

		    }
		}
	}
	Component {
		id: settingsPageComponent
		SettingsPage {
			id: settingsPage
		}
	}


	Sheet {
		id: statisticsGatherer

		acceptButtonText: qsTr("Send")
		rejectButtonText: qsTr("Cancel")

		content: Flickable {
			anchors.fill: parent
			anchors.leftMargin: 10
			anchors.rightMargin: 10
			anchors.topMargin: 10
			flickableDirection: Flickable.VerticalFlick
			Column {
			    id: col2
			    anchors.top: parent.top
			    anchors.left: parent.left
			    anchors.right: parent.right
			    spacing: 10
			    CheckBox{
				    id:submitBox
				    anchors.left: parent.left
				    anchors.right: parent.right
				    text: qsTr("Would you like to send details about your current setup?")

			    }
			    CheckBox{
				    id:dontAskLater
				    anchors.left: parent.left
				    anchors.right: parent.right
				    text: qsTr("Dont's ask me later")
			    }
			    Label
			    {
				    anchors.left: parent.left
				    anchors.right: parent.right
				    text: qsTr("Information to be transferred to the qutIM's authors:")
			    }

			    TextArea
			    {
				    anchors.left: parent.left
				    anchors.right: parent.right
				    text:statistics.infoHtml
			    }
			}
		    }

		onAccepted: statistics.setDecisition(!submitBox.checked, dontAskLater.checked);
	}

	Component.onCompleted: {
		if (statistics.action == Statistics.NeedToAskInit
				|| statistics.action == Statistics.NeedToAskUpdate) {
			statisticsGatherer.open();
		}
	}

}
