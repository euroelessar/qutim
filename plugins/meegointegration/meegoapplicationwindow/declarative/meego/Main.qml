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
    property variant applicationWindow:  serviceManager.applicationWindow
    property variant contactInfo: serviceManager.contactInfo
	property variant contactList:  serviceManager.contactList
	property variant chat:  serviceManager.chatLayer
	property variant settings:  serviceManager.settingsLayer
    property variant vibration:  serviceManager.vibration
    property variant popup:  serviceManager.popup
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
		target: root.contactInfo
        onShowRequest: {
            console.log("showRequest")
            pageStack.push(contactInfoPageComponent)
        }
    }
    Connections {
		target: root.settings
        onSettingsRequested: pageStack.push(settingsPageComponent, { "model": model })
	}
	Connections {
		target: application
        onDialogShown: {
            inputDialog.widget = widget;
            inputDialog.open();
        }
		onWidgetShown: root.pageStack.push(proxyPageComponent, { "widget": widget })
		onWidgetClosed: root.pageStack.pop(proxyPageComponent)
	}
    Connections {
		target: root.popup
        onRequestChannelList: {
            tabGroup.currentTab = channelListTab;
            // Is there any better way to make window active?..
            Qt.openUrlExternally("file:///usr/share/applications/qutim.desktop");
        }
        onRequestChannel: {
            root.chat.activeSession = channel;
            tabGroup.currentTab = chatTab;
            Qt.openUrlExternally("file:///usr/share/applications/qutim.desktop");
        }
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
    Component {
		id: contactInfoPageComponent
		ContactInfoPage {
            contactInfo: root.contactInfo
		}
	}
	Statistics {
		id: statistics
	}
    InputDialog {
		id: inputDialog
	}
	PasswordDialog {
		id: passwordDialog
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
                    platformIconId: "toolbar-new-chat" + (enabled ? "" : "-dimmed")
					tab: chatTab
					enabled: chat.activeSession !== null
				}
				TabIcon {
					platformIconId: "toolbar-list" + (enabled ? "" : "-dimmed")
					tab: conferenceUsersTab
					enabled: chat.activeSession !== null && chat.activeSession.unit.conference
				}
			}

			ToolIcon {
                id: menuIcon
				property variant menu: tabGroup.currentTab.menu
				platformIconId: "toolbar-view-menu"
                onClicked: (menu === undefined ? mainMenu : menu).open()
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

        acceptButtonText: submitBox.checked ? qsTr("Send") : qsTr("Don't send")
		rejectButtonText: qsTr("Cancel")

        content: FlickableColumn {
            anchors.fill: parent
			anchors.margins: 10
            spacing: 10
            
            CheckBox {
                id:submitBox
                width: parent.width
                text: qsTr("Would you like to send details about your current setup?")
                checked: true

            }
            CheckBox {
                id:dontAskLater
                width: parent.width
                text: qsTr("Dont's ask me later")
                checked: false
            }
            Label {
                width: parent.width
                text: qsTr("Information to be transferred to the qutIM's authors:")
            }
            TextArea {
                width: parent.width
                text: statistics.infoHtml
                readOnly: true
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
