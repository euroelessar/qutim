/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef DBUSPLUGIN_H
#define DBUSPLUGIN_H

#include <qutim/plugin.h>
#include <QDBusConnection>

using namespace qutim_sdk_0_3;

class DBusPlugin : public Plugin
{
	Q_OBJECT
public:
    explicit DBusPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QDBusConnection *m_dbus;
};

#endif // DBUSPLUGIN_H
