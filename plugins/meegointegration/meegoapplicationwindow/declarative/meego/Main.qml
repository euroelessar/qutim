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

PageStackWindow {
	id: root
	ServiceManager {
		id: serviceManager
	}
	property variant contactList:  serviceManager.contactList
	property variant chat:  serviceManager.chatLayer
	Connections {
		target: root.chat
		onActiveSessionChanged: {
			if (root.chat.activeSession)
				tabGroup.currentTab = chatTab
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
	        Page {
				id: conferenceUsersTab
	            Column {
	                spacing: 10
	   
	                Text {
	                    text: "TODO: Implement user list"
	                }
	            }
			}
	        Page {
	            id: settingsTab
	            Column {
	                spacing: 10
	   
	                Text {
	                    text: "TODO: Implement settings"
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
//					visible: chat.activeSession !== null
				}
				TabIcon {
					platformIconId: "toolbar-new-chat"
					tab: conferenceUsersTab
					visible: false
				}
				TabIcon {
					platformIconId: "toolbar-settings"
					tab: settingsTab
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
				onClicked: (menu.status == DialogStatus.Closed)
						   ? menu.open()
						   : menu.close()
			}
		}
	}
}
