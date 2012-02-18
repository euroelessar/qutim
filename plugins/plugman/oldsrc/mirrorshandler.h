/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef MIRRORSHANDLER_H
#define MIRRORSHANDLER_H

#include <QObject>
#include "plugpackagehandler.h"

class mirrorsHandler : public QObject
{
	Q_OBJECT
public:
	QList<mirrorInfo> readMirrorList(const QString &path = "");
	mirrorInfo getMirrorInfo(const QString &name);
	void registerMirror(const mirrorInfo &mirror);
	void removeMirror(const QString &name);
private:
	QDomElement createElement(const mirrorInfo &mirror);
};

#endif // MIRRORSHANDLER_H

