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
#include <QHash>
#include <QString>

class VWallSession;
class VConnection;
class VContact;
class VAccount;
class VAccountPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VAccount)
public:
	QString name;
	QString uid;
	QHash<QString, VWallSession*> walls;
	VConnection *connection;
	VAccount *q_ptr;
public slots:
	void onWallDestroyed(QObject *wall);
};

#endif // VACCOUNT_P_H
