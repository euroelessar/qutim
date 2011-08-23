import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3

Page {
	id: root
//	property alias contactList : menu.controller
	property variant contactList
	property variant chat
//	tools: ToolBarLayout {
////        ToolIcon {
////			iconId: "icon-m-toolbar-back"
////			onClicked: pageStack.pop();
////		}
////        ToolIcon {
////			NotificationBackend {
////				id: sound
////				type: "Sound"
////			}
////			platformIconId: sound.enabled ? "toolbar-volume" : "toolbar-volume-off"
////			onClicked: sound.enabled = !sound.enabled;
////		}
//        ToolIcon {
//			id: accountStatusTool
//			property Sheet accountList
//			property Sheet accountMenu
//			function closeAll() {
//				accountMenu.destroy()
//				accountList.destroy()
//			}
////			platformIconId: contactList.statusIcon(contactList.status)
//			platformIconId: "toolbar-contact"
//			onClicked: {
//				accountList = accountListComponent.createObject(root)
//				accountList.open()
//			}
//		}
//        ToolIcon {
//			ControlledMenu {
//				id: menu
//				visualParent: root.pageStack
//			}
//			platformIconId: "toolbar-view-menu"
//			onClicked: (menu.status == DialogStatus.Closed)
//					   ? menu.open()
//					   : menu.close()
//		}
//    }
	ContactListModel {
		id: listModel
	}
//	Column {
//		anchors.fill: parent
//		spacing: 2
		ListView {
			id: listView
			width: parent.width
			anchors.top: parent.top
			anchors.bottom: accountStatusTool.top
			model: listModel
			delegate: ListDelegate {
				onClicked: {
					root.chat.session(model.contact).active = true
				}
			}
			section.property: "title"
			section.criteria: ViewSection.FirstCharacter
		}
		Button {
			id: accountStatusTool
			width: parent.width
			anchors.bottom: parent.bottom
			property Sheet accountList
			property Sheet accountMenu
			function closeAll() {
				accountMenu.destroy()
				accountList.destroy()
			}
			iconSource: contactList.statusUrl(contactList.status)
			text: contactList.statusName(contactList.status)
			onClicked: {
				accountList = accountListComponent.createObject(root)
				accountList.open()
			}
		}
//	}
	SectionScroller {
        listView: listView
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
							text: modelData.name
							iconSource: contactList.statusUrl(modelData.status)
							onClicked: {
								var object = accountMenuComponent.createObject(accountListContent);
								accountStatusTool.accountMenu = object;
								object.controller = modelData;
								object.open()
							}
						}
					}
	            }
			}
		}
	}
	Component {
		id: accountMenuComponent
		Sheet {
			id: accountMenuContent
			property alias controller: accountMenuModel.controller
			rejectButtonText: qsTr("Cancel")
			onRejected: accountMenuContent.destroy()
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
						model: MenuModel {
							id: accountMenuModel
						}
						Button {
							text: action.text
//									iconSource: action.icon
							onClicked: {
								action.trigger()
								accountStatusTool.closeAll()
							}
						}
					}
	            }
			}
		}
	}
}
