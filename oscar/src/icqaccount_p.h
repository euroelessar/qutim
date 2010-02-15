/****************************************************************************
 *  icqaccount_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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


#ifndef ICQACCOUNT_P_H
#define ICQACCOUNT_P_H

#include "icqaccount.h"
#include "clientidentify.h"
#include <QTimer>

namespace Icq {

struct IcqAccountPrivate
{
	OscarConnection *conn;
	Feedbag *feedbag;
	QString name;
	bool avatars;
	QHash<quint64, Cookie*> cookies;
	Capabilities caps;
	QHash<QString, Capability> typedCaps;
	Status lastStatus;
	QTimer reconnectTimer;
	QHash<QString, IcqContact *> contacts;
	QList<RosterPlugin*> rosterPlugins;
	ClientIdentify *identify;
};

}

#endif // ICQACCOUNT_P_H
