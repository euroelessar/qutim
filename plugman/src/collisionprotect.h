/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

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
