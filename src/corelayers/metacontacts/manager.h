/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef MANAGER_H
#define MANAGER_H

#include <qutim/metacontactmanager.h>
#include "metacontactimpl.h"

namespace Core
{
namespace MetaContacts
{
class Manager : public qutim_sdk_0_3::MetaContactManager
{
	Q_OBJECT
public:
	Manager();
	virtual ~Manager();
	virtual qutim_sdk_0_3::ChatUnit *getUnit(const QString &unitId, bool create = false);
	void removeContact(const QString &id) { m_contacts.remove(id); }
	virtual QString name() const;
protected:
	virtual void loadContacts();
private slots:
	void onSplitTriggered(QObject*);
	void onCreateTriggered(QObject*);
private:
	QHash<QString, MetaContactImpl*> m_contacts;
};
}
}

#endif // MANAGER_H
