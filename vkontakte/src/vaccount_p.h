/****************************************************************************
 *  vaccount_p.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef VACCOUNT_P_H
#define VACCOUNT_P_H
#include <QList>
#include <QHash>

class VConnection;
class VContact;
struct VAccountPrivate
{
	QString name;
	QString uid;
	QHash<QString, VContact*> contactsHash;
	QList<VContact *> contactsList;
	VConnection *connection;
};

#endif // VACCOUNT_P_H
