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

.pragma library

// From com.nokia.meego.ToolIcon
function handleIconSource(iconId) {
    var prefix = "icon-m-"
    // check if id starts with prefix and use it as is
    // otherwise append prefix and use the inverted version if required
    if (iconId.indexOf(prefix) !== 0)
        iconId =  prefix.concat(iconId); // .concat(theme.inverted ? "-white" : "");
    return "image://theme/" + iconId;
}
