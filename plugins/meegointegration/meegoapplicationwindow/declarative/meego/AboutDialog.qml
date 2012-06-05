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
import com.nokia.extras 1.0
import org.qutim 0.3

Sheet {
	id: aboutDialog
	AboutDialogWrapper {
		id: handler
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

		Column {
			anchors.right: parent.right
			anchors.left: parent.left
			anchors.top: parent.top

			id:programLine
			spacing: 10

			Text {
				anchors.right: parent.right
				id: versionText
				text: handler.version
			}

			Text {
				anchors.right: parent.right
				anchors.top: versionText.bottom
				id:programName
				font.pointSize: 60
				text: "qutIM"
			}

		}


//		ListModel {
//			id:linksModel

//			ListElement {
//				name: qsTr("Project site")
//				url: "http://qutim.org"
//			}
//			ListElement {
//				name: sqTr("Source code repository")
//				url: "http://github.com/euroelessar"
//			}
//			ListElement {
//				name: qsTr("Bug tracker")
//				url: "http://trac.qutim.org"
//			}
//		}

		ListView {
			id: mainInfo
			spacing: 20
			anchors.topMargin:40
			anchors{top:parent.top; left:parent.left; right:parent.right;bottom:parent.bottom}
			interactive: false
			model: ListModel { }
			delegate: Text {
				anchors{left: parent.left; right: parent.right}
				text: name + ": <br> <a href=\"" + url + "\">" + url  +"</a>"
				wrapMode: Text.WordWrap
				textFormat: Text.RichText
				onLinkActivated: {
					Qt.openUrlExternally(link);
				}
			}
			
			Component.onCompleted: {
				mainInfo.model.append({
					name: qsTr("Project site"),
					url: "http://qutim.org"
				});
				mainInfo.model.append({
					name: qsTr("Source code repository"),
					url: "http://github.com/euroelessar"
				});
				mainInfo.model.append({
					name: qsTr("Bug tracker"),
					url: "http://trac.qutim.org"
				});
			}
		}

		Column {
			id:additionalInfo
			anchors{bottom: parent.bottom; right:parent.right;}
			spacing:10


			Button {
				anchors.right:parent.right;
				text:qsTr("Developers")
				onClicked: {pageStack.push(developersPage);toolBar.__currentContainer.visible = true;}
			}
			Button {
				anchors.right:parent.right;
				text:qsTr("Translators")
				onClicked: {pageStack.push(translatorsPage);toolBar.__currentContainer.visible = true;}
			}
			Button {
				anchors.right:parent.right;
				text:qsTr("License")
				onClicked: {pageStack.push(licensePage);toolBar.__currentContainer.visible = true;}
			}
		}



	}

	Page
	{
		id:developersPage
		Flickable {
			anchors.fill: parent
			anchors.leftMargin: 10
			anchors.topMargin: 10
			contentWidth: developersText.width
			contentHeight: developersText.height
			flickableDirection: Flickable.VerticalFlick
			Text {
				id:developersText
				width: parent.width
				text: handler.developers
				wrapMode: Text.WordWrap
				textFormat: Text.RichText
				onLinkActivated: {
					Qt.openUrlExternally(link);
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

	Page
	{
		id:translatorsPage
		Flickable {
			anchors.fill: parent
			anchors.leftMargin: 10
			anchors.topMargin: 10
			contentWidth: translatorsText.width
			contentHeight: translatorsText.height
			flickableDirection: Flickable.VerticalFlick
			Text {
				id:translatorsText
				width: parent.width
				text: handler.translators
				wrapMode: Text.WordWrap
				textFormat: Text.RichText
				onLinkActivated: {
					Qt.openUrlExternally(link);
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

	Page
	{
		id:licensePage
		Flickable {
			anchors.fill: parent
			anchors.leftMargin: 10
			anchors.topMargin: 10
			contentWidth: licensePage.width
			contentHeight: licenseText.height
			flickableDirection: Flickable.VerticalFlick
			Text {
				width: parent.width
				id:licenseText
				text: handler.license
				wrapMode: Text.WordWrap
				textFormat: Text.RichText
				onLinkActivated: {
					Qt.openUrlExternally(link);
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
		pageStack.push(mainPage);
	}
}
