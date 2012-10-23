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
class SimpleRosterStorage : public Ureen::RosterStorage
{
	Q_OBJECT
public:
    SimpleRosterStorage();
    ~SimpleRosterStorage();
	
	virtual QString load(Ureen::Account *account);
	virtual void addContact(Ureen::Contact *contact, const QString &version = QString());
	virtual void updateContact(Ureen::Contact *contact, const QString &version = QString());
	virtual void removeContact(Ureen::Contact *contact, const QString &version = QString());
private:
	struct AccountContext
	{
		QMap<Ureen::Contact*, int> indexes;
		QList<int> freeIndexes;
	};
	QMap<Ureen::Account*, AccountContext> m_contexts;
};
}

#endif // SIMPLEROSTERSTORAGE_H

