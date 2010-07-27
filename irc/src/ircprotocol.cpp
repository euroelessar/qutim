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
#include "ircchannel.h"
#include "ircchannelparticipant.h"
#include "ircconnection.h"
#include <QStringList>
#include <QPointer>
#include <qutim/actiongenerator.h>
#include <qutim/statusactiongenerator.h>
#include <QRegExp>

Q_DECLARE_METATYPE(qutim_sdk_0_3::irc::IrcAccount*);

namespace qutim_sdk_0_3 {

namespace irc {

IrcProtocol *IrcProtocol::self = 0;

IrcCommandAlias::IrcCommandAlias(const QString &_name, const QString &_command, Types _types) :
	name(_name), command(_command), types(_types)
{
}

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
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	// Register actions.
	ActionGenerator *gen = new ActionGenerator(QIcon(),
					QT_TRANSLATE_NOOP("IRC", "Show console..."),
					SLOT(showConsole()));
	gen->setPriority(35);
	gen->setType(ActionTypeContactList | 0x2000);
	MenuController::addAction<IrcAccount>(gen);
	gen = new ActionGenerator(QIcon(),
					QT_TRANSLATE_NOOP("IRC", "Search channel..."),
					SLOT(showChannelList()));
	gen->setPriority(35);
	gen->setType(ActionTypeContactList | 0x2000);
	MenuController::addAction<IrcAccount>(gen);
	// Register status actions.
	Status status(Status::Online);
	status.initIcon("irc");
	MenuController::addAction<IrcAccount>(new StatusActionGenerator(status));
	status.setType(Status::Away);
	status.initIcon("irc");
	MenuController::addAction<IrcAccount>(new StatusActionGenerator(status));
	status.setType(Status::Offline);
	status.initIcon("irc");
	MenuController::addAction<IrcAccount>(new StatusActionGenerator(status));
	// Load accounts.
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

ChatSession *IrcProtocol::activeSession() const
{
	return d->activeSession;
}

void IrcProtocol::registerCommandAlias(const IrcCommandAlias &alias)
{
	IrcConnection::registerAlias(alias);
}

void IrcProtocol::removeCommandAlias(const QString &name)
{
	IrcConnection::removeAlias(name);
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
			return false;
		case ProtocolSupportGroupChat:
			return true;
		default:
			return QVariant();
	}
}

void IrcProtocol::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(activated(bool)), this, SLOT(onSessionActivated(bool)));
}

void IrcProtocol::onSessionActivated(bool active)
{
	ChatSession *session = qobject_cast<ChatSession*>(sender());
	if (!active && session == d->activeSession)
		d->activeSession = 0;
	else if (session && active && qobject_cast<IrcChannel*>(session->getUnit()))
		d->activeSession = session;
}

} } // namespace qutim_sdk_0_3::irc
