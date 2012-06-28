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
	property variant chat
	ListView {
		id: listView
		model: ChatChannelModel {
			
		}
		anchors.fill: parent
		delegate: ItemDelegate {
			title: channel.unit.title
			subtitle: channel.unit.id
			iconSource: __suggestIcon(channel.unit, channel.unreadCount)
			onClicked: {
                chat.activeSession = channel;
				channel.showChat();
			}
			function __suggestIcon(unit, unreadCount) {
                var iconId = "icon-m-";
				if (unreadCount > 0) {
					iconId += "content-sms";
				} else if (unit.conference) {
					iconId += "content-chat";
				} else {
//					var filePath = unit.avatar;
//					if (filePath === undefined || filePath == "")
						iconId += "content-avatar-placeholder";
//					else
//						return "file://" + filePath;
				}
				if (theme.inverse)
					iconId += "-inverse";
				return "image://theme/" + iconId;
			}
			ToolIcon {
				id: closeButton
				anchors { verticalCenter: parent.verticalCenter; right: parent.right }
				platformIconId: "toolbar-close"
                onClicked: {
                    if (unit.conference)
                        unit.leave();
                    channel.close()
                }
			}
		}
	}
}
