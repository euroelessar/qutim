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

#ifndef MANAGER_H
#define MANAGER_H

#include <qutim/metacontactmanager.h>
#include "metacontactimpl.h"
#include "messagehandler.h"

namespace qutim_sdk_0_3 {
class RosterStorage;
}

namespace Core
{
namespace MetaContacts
{

class Factory;
class Manager : public qutim_sdk_0_3::MetaContactManager
{
	Q_OBJECT
	Q_CLASSINFO("Uses", "RosterStorage")
public:
	Manager();
	virtual ~Manager();
	virtual qutim_sdk_0_3::ChatUnit *getUnit(const QString &unitId, bool create = false);
	void removeContact(const QString &id) { m_contacts.remove(id); }
	virtual QString name() const;
protected:
	virtual void loadContacts();
private slots:
	void initActions();
	void onSplitTriggered(QObject*);
	void onCreateTriggered(QObject*);
	void onContactCreated(qutim_sdk_0_3::Contact*);
private:
	QHash<QString, MetaContactImpl*> m_contacts;
	qutim_sdk_0_3::RosterStorage *m_storage;
	QScopedPointer<Factory> m_factory;
	friend class Factory;
	bool m_blockUpdate;
	QScopedPointer <MetaContactMessageHandler> m_handler;
};
}
}

#endif // MANAGER_H

