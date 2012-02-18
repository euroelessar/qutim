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
#ifndef STATUSACTIONGENERATOR_P_H
#define STATUSACTIONGENERATOR_P_H

#include "actiongenerator_p.h"
#include "statusactiongenerator.h"

namespace qutim_sdk_0_3
{
class StatusActionGeneratorPrivate : public ActionGeneratorPrivate
{
public:
	Status status;
	Status::Type statusType;
};

class StatusActionHandler : public QObject
{
	Q_OBJECT
public:
	StatusActionHandler(QObject *parent = 0);
	QByteArray memberName() { return m_memberName; }
public slots:
	void changeStatus(QAction *action,QObject *controller);
private:
	QByteArray m_memberName;
};
}

#endif // STATUSACTIONGENERATOR_P_H

