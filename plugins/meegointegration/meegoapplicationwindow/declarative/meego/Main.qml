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

//	property Item contactListPage: ContactList {
//		contactList: root.contactList
//		chat: root.chat
//	}
//	property Item chatPage: Chat {
//		chat: root.chat
//	}
//	property Item chatChannelListPage: ChatChannelList {
//		chat: root.chat
//	}
//	initialPage: contactListPage
	initialPage: Page {
		TabGroup {
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
				Menu {
					id: menu
					visualParent: root.pageStack
				}
				platformIconId: "toolbar-view-menu"
				onClicked: (menu.status == DialogStatus.Closed)
						   ? menu.open()
						   : menu.close()
			}
		}
	}
}
