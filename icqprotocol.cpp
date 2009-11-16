/****************************************************************************
 *  icqprotocol.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "icq_global.h"
#include "icqprotocol.h"
#include "icqaccount.h"
#include <QStringList>
#include <QPointer>
#include <QDebug>

IcqProtocol *IcqProtocol::self = 0;

struct IcqProtocolPrivate
{
	inline IcqProtocolPrivate() : accounts_hash(new QHash<QString, QPointer<IcqAccount> >()) {}
	inline ~IcqProtocolPrivate() { delete accounts_hash; }
	union {
		QHash<QString, QPointer<IcqAccount> > *accounts_hash;
		QHash<QString, IcqAccount *> *accounts;
	};
};

qutim_sdk_0_3::Status icqStatusToQutim(quint16 status)
{
	if(status & IcqFlagInvisible)
		return Invisible;
	else if(status & IcqFlagEvil)
		return Evil;
	else if(status & IcqFlagDepress)
		return Depression;
	else if(status & IcqFlagHome)
		return AtHome;
	else if(status & IcqFlagWork)
		return AtWork;
	else if(status & IcqFlagLunch)
		return OutToLunch;
	else if(status & IcqFlagDND)
		return DND;
	else if(status & IcqFlagOccupied)
		return Occupied;
	else if(status & IcqFlagNA)
		return NA;
	else if(status & IcqFlagAway)
		return Away;
	else if(status & IcqFlagFFC)
		return FreeChat;
	return Online;
}

IcqProtocol::IcqProtocol() : p(new IcqProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
}

IcqProtocol::~IcqProtocol()
{
	self = 0;
}

AccountCreationWizard *IcqProtocol::accountCreationWizard()
{
	return 0;
}

void IcqProtocol::loadAccounts()
{
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uin, accounts)
		p->accounts_hash->insert(uin, new IcqAccount(uin));
}

QList<Account *> IcqProtocol::accounts() const
{
    QList<Account *> accounts;
	QHash<QString, QPointer<IcqAccount> >::const_iterator it;
	for (it=p->accounts_hash->begin();it!=p->accounts_hash->end();it++)
		accounts.append(it.value());
    return accounts;
}
Account *IcqProtocol::account(const QString &id) const
{
    return p->accounts_hash->value(id);
}
