/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef MACWIDGET_P_H
#define MACWIDGET_P_H
#include "macwidget.h"
#include <qutim/servicemanager.h>

namespace Core {
namespace SimpleContactList {

class FileMenuController : public MenuController
{
	Q_OBJECT
public:
	FileMenuController(QObject *parent) : MenuController(parent)
	{
		if (MenuController *contactList = ServiceManager::getByName<MenuController *>("ContactList"))
			setMenuOwner(contactList);
	}
};

class RosterMenuController : public MenuController
{
	Q_OBJECT
public:
	RosterMenuController(QObject *parent) : MenuController(parent)
	{
	}
};

}
}

#endif // MACWIDGET_P_H

