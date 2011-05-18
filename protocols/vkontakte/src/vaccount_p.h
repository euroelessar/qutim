/****************************************************************************
 *  vaccount_p.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <QHash>
#include <QString>

class VWallSession;
class VConnection;
class VContact;
class VAccount;
class VAccountPrivate
{
	Q_DECLARE_PUBLIC(VAccount)
public:
	QString name;
	QString uid;
	QHash<QString, VWallSession*> walls;
	VConnection *connection;
	VAccount *q_ptr;
};

#endif // VACCOUNT_P_H
