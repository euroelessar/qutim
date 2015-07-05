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

Menu {
	id: root
	property QtObject controller
    property QtObject menuBuilder: MenuBuilder {
        id: builder
        // Reference to Column inside MenuLayout
        menu: menuLayout.children[0]
        controller: root.controller
    }
    MenuLayout {
        id: menuLayout
    }
}
