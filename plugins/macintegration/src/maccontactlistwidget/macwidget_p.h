/****************************************************************************
 *  macwidget_p.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
