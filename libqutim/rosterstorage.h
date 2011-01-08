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

#ifndef ROSTERSTORAGE_H
#define ROSTERSTORAGE_H

#include "libqutim_global.h"
#include <QVariantMap>

namespace qutim_sdk_0_3
{
class RosterStoragePrivate;
class Contact;
class Account;

class LIBQUTIM_EXPORT ContactsFactory
{
public:
	virtual ~ContactsFactory();

	virtual Contact *addContact(const QString &id, const QVariantMap &data) = 0;
	virtual void serialize(Contact *contact, QVariantMap &data) = 0;
};

class LIBQUTIM_EXPORT RosterStorage : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(RosterStorage)
	Q_CLASSINFO("Service", "RosterStorage")
public:
	static RosterStorage *instance();

	virtual QString load(Account *account) = 0;
	virtual void addContact(Contact *contact, const QString &version = QString()) = 0;
	virtual void updateContact(Contact *contact, const QString &version = QString()) = 0;
	virtual void removeContact(Contact *contact, const QString &version = QString()) = 0;
protected:
    RosterStorage();
	virtual ~RosterStorage();
private:
	QScopedPointer<RosterStoragePrivate> d_ptr;
};
}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::ContactsFactory, "org.qutim.ContactsFactory")

#endif // ROSTERSTORAGE_H
