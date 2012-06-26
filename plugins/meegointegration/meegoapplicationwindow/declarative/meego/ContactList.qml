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
import "constants.js" as UI

Page {
	id: root
	property variant contactList
	property variant showOffline:false
	property variant chat
	property variant menu: ControlledMenu {
		controller: contactList
	}
//    MaskedItem {
//        id: maskedItem
//        visible: false
//        mask: 
//    }

	ContactListModel {
		id: listModel
		filter: filterField.text
		showOffline: root.showOffline
		statusPrefix: "icon-m"
	}
	ListView {
		id: listViewItem
		width: parent.width
		anchors.top: parent.top
		anchors.bottom: accountStatusTool.top
		model: listModel
        delegate: Item {
            id: listItem
            height: UI.LIST_ITEM_HEIGHT
            width: parent.width
            clip: true
            property bool hasAvatar: model.avatar != ""
            
            property int titleSize: UI.LIST_TILE_SIZE
            property int titleWeight: Font.Bold
            property color titleColor: theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED : UI.LIST_TITLE_COLOR
        
            property int subtitleSize: UI.LIST_SUBTILE_SIZE
            property int subtitleWeight: Font.Light
            property color subtitleColor: theme.inverted ? UI.LIST_SUBTITLE_COLOR_INVERTED : UI.LIST_SUBTITLE_COLOR
            
            BorderImage {
                id: background
                anchors.fill: parent
                // Fill page porders
                anchors.leftMargin: -UI.MARGIN_XLARGE
                anchors.rightMargin: -UI.MARGIN_XLARGE
                visible: mouseArea.pressed
                source: "image://theme/meegotouch-list-background-pressed-center"
            }
            
            
            Row {
                anchors.fill: parent
                spacing: UI.LIST_ITEM_SPACING
                
                Item {
                    id: avatarImage
                    anchors.verticalCenter: parent.verticalCenter
                    width: UI.LIST_ICON_SIZE
                    height: UI.LIST_ICON_SIZE
//                    source: listItem.hasAvatar ? "image://theme/meegotouch-avatar-frame-small" : ""
                    Image {
                        anchors.fill: parent
                        MaskEffect {
                            id: maskEffect
                            mask: Image {
                                source: "image://theme/meegotouch-avatar-mask-small"
                            }
                        }
                        effect: listItem.hasAvatar ? maskEffect : null
                        source: listItem.hasAvatar ? model.avatar : "image://theme/icon-m-content-avatar-placeholder"
                    }
                    Image {
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        width: UI.LIST_ICON_SIZE / 2
                        height: UI.LIST_ICON_SIZE / 2
                        source: model.iconSource
                    }
                }
                
                Column {
                    anchors.verticalCenter: parent.verticalCenter
        
                    Label {
                        id: mainText
                        text: model.title
                        font.weight: listItem.titleWeight
                        font.pixelSize: listItem.titleSize
                        color: listItem.titleColor
                        maximumLineCount: 1
                    }
        
                    Label {
                        id: subText
                        text: model.subtitle
                        font.weight: listItem.subtitleWeight
                        font.pixelSize: listItem.subtitleSize
                        color: listItem.subtitleColor
                        maximumLineCount: 1
        
                        visible: text != ""
                    }
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    root.chat.session(model.contact).active = true
                    root.chat.show()
                }
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
