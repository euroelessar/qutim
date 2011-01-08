/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
