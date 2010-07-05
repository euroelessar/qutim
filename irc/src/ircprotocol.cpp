/****************************************************************************
 *  Ircprotocol.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircprotocol_p.h"
#include "ircaccount.h"
#include <QStringList>
#include <QPointer>

namespace qutim_sdk_0_3 {

namespace irc {

IrcProtocol *IrcProtocol::self = 0;

IrcProtocol::IrcProtocol() :
	d(new IrcProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
	updateSettings();
}

IrcProtocol::~IrcProtocol()
{
	self = 0;
}

void IrcProtocol::loadAccounts()
{
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &network, accounts) {
		IrcAccount *acc = new IrcAccount(network);
		d->accounts_hash->insert(network, acc);
		acc->updateSettings();
		emit accountCreated(acc);
	}
}

QList<Account *> IrcProtocol::accounts() const
{
	QList<Account *> accounts;
	QHash<QString, QPointer<IrcAccount> >::const_iterator it;
	for (it = d->accounts_hash->begin(); it != d->accounts_hash->end(); it++)
		accounts.append(it.value());
	return accounts;
}

Account *IrcProtocol::account(const QString &id) const
{
	return d->accounts_hash->value(id);
}

IrcAccount *IrcProtocol::getAccount(const QString &id, bool create)
{
	IrcAccount *account = d->accounts_hash->value(id).data();
	if (!account && create) {
		account = new IrcAccount(id);
		d->accounts_hash->insert(id, account);
		emit accountCreated(account);
	}
	return account;
}

void IrcProtocol::updateSettings()
{
	foreach (QPointer<IrcAccount> acc, *d->accounts_hash)
		acc->updateSettings();
}

QVariant IrcProtocol::data(DataType type)
{
	switch (type) {
		case ProtocolIdName:
			return "Nick";
		case ProtocolContainsContacts:
			return true;
		default:
			return QVariant();
	}
}
} } // namespace qutim_sdk_0_3::irc
