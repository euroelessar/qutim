/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

Item {
    // The currently selected tab.
    property Item currentTab
    
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0
    
    onVisibleChanged: __init()
    onChildrenChanged: __init()
    onCurrentTabChanged: __update()
	
	PropertyAnimation {
		id: animationTo
		duration: 250
		property: "x"
		easing.type: Easing.InOutCubic
		onCompleted: {
			target.status = PageStatus.Active
		}
	}
	PropertyAnimation {
		id: animationFrom
		duration: animationTo.duration
		property: animationTo.property
		easing.type: animationTo.easing.type
		onCompleted: {
			target.status = PageStatus.Inactive
			target.visible = false;
		}
	}
	
	function __init() {
        for (var i = 0; i < children.length; i++) {
            var child = children[i];
            var newVis = child == currentTab;
            if (child.visible != newVis) {
                child.visible = newVis;
                if (child.status !== undefined) {
                    if (newVis) {
                        child.status = PageStatus.Activating;
                        child.status = PageStatus.Active;
                    } else {
                        child.status = PageStatus.Deactivating;
                        child.status = PageStatus.Inactive;
                    }
                }
            }
		}
	}
	
    function __update() {
		var from = -1, to = -1;
        for (var i = 0; i < children.length; i++) {
            var child = children[i];
			if (child == currentTab)
				to = i;
			if (child.visible)
				from = i;
        }
		if (from == -1 || to == -1) {
			__init();
			return;
		}

		if (from != to) {
			var start = (from > to) ? -width : width;
			from = children[from];
			to = children[to];
			to.x = start
			if (from.status !== undefined)
				from.status = PageStatus.Deactivating
			if (to.status !== undefined)
				to.status = PageStatus.Activating
			to.visible = true;
			animationFrom.from = 0;
			animationFrom.to = -start;
			animationFrom.target = from;
			animationTo.from = start;
			animationTo.to = 0;
			animationTo.target = to;
			animationFrom.start();
			animationTo.start();
		}
    }
}
