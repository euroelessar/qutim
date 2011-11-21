/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef COLLISIONPROTECT_H
#define COLLISIONPROTECT_H

#include <QObject>
#include <QStringList>

class CollisionProtect : public QObject
{
Q_OBJECT
public:
	CollisionProtect(const QString &prefix = QString ());
	bool checkPackageFiles(const QStringList &files); 
 	/*!true - нет столкновений, false - есть столкновения*/
private:
	QString prefix;
};

#endif // COLLISIONPROTECT_H

