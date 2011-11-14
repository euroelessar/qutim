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

