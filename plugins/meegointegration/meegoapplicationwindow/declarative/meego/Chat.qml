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

import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import QtWebKit 1.0
import "messageDelegate"

Page {
	id: root
	property variant chat
	property variant menu: ControlledMenu {
		controller: chat.activeSession ? chat.activeSession.unit : null
	}
    property variant currentSessionPage
    Connections {
		target: root.chat
        onActiveSessionChanged: {
            var session = root.chat.activeSession;
            if (!session.page) {
                session.page = webViewComponent.createObject(root, { "session": session });
            }
            root.currentSessionPage = session.page;
        }
	}
    Component {
        id: webViewComponent
        Item {
            id: item
            anchors.top: root.top
            anchors.bottom: textField.top
            anchors.left: root.left
            anchors.right: root.right
            visible: item === root.currentSessionPage
            property QtObject session

            Flickable {
                id: flickable
                anchors.fill: parent
                contentWidth: Math.max(root.width, webView.width)
                contentHeight: Math.max(root.height, webView.height)
                pressDelay: 200
                clip: true
                onHeightChanged: flickable.scrollToBottom()
                
                function scrollToBottom() {
                    flickable.contentY = flickable.contentHeight - flickable.height;
                }
                
                WebView {
                    id: webView
                    preferredWidth: flickable.width
                    preferredHeight: flickable.height
                    onAlert: console.log(message)
                    settings.defaultFontSize: controller.fontSize
                    settings.standardFontFamily: controller.fontFamily
                    settings.fixedFontFamily: controller.fontFamily
                    settings.serifFontFamily: controller.fontFamily
                    settings.sansSerifFontFamily: controller.fontFamily
                    javaScriptWindowObjects: ChatController {
                        id: controller
                        WebView.windowObjectName: "client"
                        webView: webView
                        session: item.session
                    }
                    Connections {
                        target: controller.session
                        onMessageAppended: controller.append(message)
                    }
                    onHeightChanged: flickable.scrollToBottom()
                }
            }
            ScrollDecorator {
                flickableItem: flickable
            }
        }
    }

//	Connections {
//		target: root.chat
//		onActiveSessionChanged: {
//			if (root.chat.activeSession)
//				tabGroup.currentTab = chatTab
//		}
//	}
	TextArea {
		id: textField
		anchors { left: parent.left; bottom: parent.bottom }
		placeholderText: qsTr("Write anything")
		height: Math.min(200, implicitHeight)
		width: parent.width - sendButton.width
	}
	ToolIcon {
		id: sendButton
		anchors { top: textField.top; right: parent.right; }
		platformIconId: "toolbar-send-chat"
		onClicked: {
			var result = chat.activeSession.send(textField.text)
			if (result != -2)
				textField.text = ""
		}
	}
}
