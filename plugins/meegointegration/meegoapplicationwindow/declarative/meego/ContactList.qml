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
import com.nokia.extras 1.0
import org.qutim 0.3

Page {
	id: root
	property variant contactList
	property variant showOffline:false
	property variant chat
	property variant menu: ControlledMenu {
		controller: contactList
	}
	ContactListModel {
		id: listModel
		filter: filterField.text
		showOffline: root.showOffline
		statusPrefix: "icon-m-common"
	}
	ListView {
		id: listViewItem
		width: parent.width
		anchors.top: parent.top
		anchors.bottom: accountStatusTool.top
		model: listModel
		delegate: ItemDelegate {
			onClicked: {
				root.chat.session(model.contact).active = true
				root.chat.show()
			}
		}
		section.property: "alphabet"
		section.criteria: ViewSection.FullString
		section.delegate: sectionHeading
		clip:true
	}
	// The delegate for each section header
	Component {
        id: sectionHeading
        Rectangle {
            width: root.width
            height: childrenRect.height
			color: Qt.rgba(0, 0, 0, 0.2)
            Text {
				anchors.right: parent.right
				anchors.rightMargin: 15
                text: section
                font.bold: true
				font.pixelSize: 20
            }
        }
    }
	TextField {
		id: filterField
		anchors { left: parent.left; bottom: parent.bottom; right: accountStatusTool.left }
		platformSipAttributes: SipAttributes { actionKeyHighlighted: true }
		placeholderText: qsTr("Search contact")
		platformStyle: TextFieldStyle { paddingRight: clearButton.width }
		Image {
			id: clearButton
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			source: "image://theme/icon-m-input-clear"
			MouseArea {
				anchors.fill: parent
				onClicked: {
					inputContext.reset();
					filterField.text = "";
				}
			}
		}
	}

	ListButton {
		id: accountStatusTool
		anchors { right: parent.right; bottom: parent.bottom }
		property Sheet accountList
		iconSource: contactList.statusUrl(contactList.status)
		text: contactList.statusName(contactList.status)
		onClicked: {
			accountList = accountListComponent.createObject(root)
			accountList.open()
		}
	}


	SectionScroller {
        listView: listViewItem
    }
	ScrollDecorator {
		flickableItem: listViewItem
	}
	Component {
		id: accountListComponent
		Sheet {
			id: accountListContent
			rejectButtonText: qsTr("Cancel")
			onRejected: accountListContent.destroy()
			content: Flickable {
				anchors.fill: parent
	            anchors.leftMargin: 10
		    anchors.topMargin: 10
		    contentWidth: accountListColumn.width
	            contentHeight: accountListColumn.height
	            flickableDirection: Flickable.VerticalFlick

	            Column {
	                id: accountListColumn
	                anchors.top: parent.top
	                spacing: 10
					Repeater {
						model: [
							Status.Online,
							Status.FreeChat,
							Status.Away,
							Status.DND,
							Status.NA,
							Status.Invisible,
							Status.Offline
						]
						Button {
							text: contactList.statusName(modelData)
							iconSource: contactList.statusUrl(modelData)
							onClicked: {
								contactList.status = modelData
								accountListContent.destroy()
							}
						}
					}
					Repeater {
						model: contactList.accounts
						Button {
							text: modelData.id
							iconSource: contactList.statusUrl(modelData.status)
							onClicked: {
								accountMenu.controller = modelData
								accountMenu.open()
							}
						}
					}
	            }
			}
			ControlledMenu {
				id: accountMenu
				visualParent: accountListContent
				onStatusChanged: {
					if (status == DialogStatus.Closing)
						accountListContent.reject()
				}
			}
		}
	}
}
