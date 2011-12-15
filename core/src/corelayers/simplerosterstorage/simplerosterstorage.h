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

#ifndef SIMPLEROSTERSTORAGE_H
#define SIMPLEROSTERSTORAGE_H

#include <qutim/rosterstorage.h>

namespace Core
{
class SimpleRosterStorage : public qutim_sdk_0_3::RosterStorage
{
	Q_OBJECT
public:
    SimpleRosterStorage();
    ~SimpleRosterStorage();
	
	virtual QString load(qutim_sdk_0_3::Account *account);
	virtual void addContact(qutim_sdk_0_3::Contact *contact, const QString &version = QString());
	virtual void updateContact(qutim_sdk_0_3::Contact *contact, const QString &version = QString());
	virtual void removeContact(qutim_sdk_0_3::Contact *contact, const QString &version = QString());
private:
	struct AccountContext
	{
		QMap<qutim_sdk_0_3::Contact*, int> indexes;
		QList<int> freeIndexes;
	};
	QMap<qutim_sdk_0_3::Account*, AccountContext> m_contexts;
};
}

#endif // SIMPLEROSTERSTORAGE_H

