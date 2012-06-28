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
	property variant unitsModel: chat.activeSession ? chat.activeSession.units : emptyModel
	ListModel {
		id: emptyModel
	}
	ListView {
		id: listViewItem
		width: parent.width
		anchors.fill: parent
		model: unitsModel
		delegate: ContactItem {
			onClicked: {
                var session = root.chat.session(model.contact);
                chat.activeSession = session;
                root.chat.show();
			}
		}
		section.property: "alphabet"
		section.criteria: ViewSection.FullString
		section.delegate: sectionHeading
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


	SectionScroller {
        listView: listViewItem
    }
	ScrollDecorator {
		flickableItem: listViewItem
	}
}
