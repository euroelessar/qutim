/****************************************************************************
 *  vkontakteprotocol.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#include "vkontakteprotocol.h"
#include "vkontakteprotocol_p.h"
#include <qutim/account.h>
#include "vaccount.h"
#include <qutim/statusactiongenerator.h>
#include "vcontact.h"
#include <QDesktopServices>
#include <QUrl>
#include <qutim/icon.h>

VkontakteProtocol *VkontakteProtocol::self = 0;

VkontakteProtocol::VkontakteProtocol() :
		d_ptr(new VkontakteProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
	d_func()->q_ptr = this;
}

VkontakteProtocol::~VkontakteProtocol()
{
	foreach (VAccount *acc, *d_func()->accounts)
		acc->saveSettings();
	self = 0;
}

Account* VkontakteProtocol::account(const QString& id) const
{
	Q_D(const VkontakteProtocol);
	return d->accounts_hash->value(id);
}
QList< Account* > VkontakteProtocol::accounts() const
{
	Q_D(const VkontakteProtocol);
	QList<Account *> accounts;
	QHash<QString, QPointer<VAccount> >::const_iterator it;
	for (it = d->accounts_hash->begin(); it != d->accounts_hash->end(); it++)
		accounts.append(it.value());
	return accounts;

}
void VkontakteProtocol::loadAccounts()
{
	Q_D(VkontakteProtocol);
	QList<Status> statuses;
	statuses << Status(Status::Online)
			<< Status(Status::Offline);
	foreach (Status status, statuses) {
		status.initIcon("vkontakte");
		Status::remember(status, "vkontakte");
		MenuController::addAction(new StatusActionGenerator(status), &VAccount::staticMetaObject);
	}

	static ActionGenerator homepage_gen(Icon("applications-internet"),
							   QT_TRANSLATE_NOOP("Vkontakte","Open homepage"),
							   d,
							   SLOT(onOpenWebPageTriggered(QObject*)));
	homepage_gen.setType(ActionTypeContactList);
	MenuController::addAction<VContact>(&homepage_gen);

	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uid, accounts) {
		VAccount *acc = new VAccount(uid);
		d->accounts_hash->insert(uid, acc);
		acc->loadSettings();
		connect(acc,SIGNAL(destroyed(QObject*)),d,SLOT(onAccountDestroyed(QObject*)));
		emit accountCreated(acc);
	}
}

QVariant VkontakteProtocol::data(DataType type)
{
	switch (type) {
		case ProtocolIdName:
			return tr("id");
		case ProtocolContainsContacts:
			return true;
		default:
			return QVariant();
	}
}

void VkontakteProtocolPrivate::onAccountDestroyed(QObject *obj)
{
	VAccount *acc = reinterpret_cast<VAccount*>(obj);
	accounts->remove(accounts->key(acc));
}

void VkontakteProtocolPrivate::onOpenWebPageTriggered(QObject *obj)
{
	VContact *con = qobject_cast<VContact*>(obj);
	Q_ASSERT(obj);
	QUrl url ("http://vkontakte.ru/id" + con->id());
	QDesktopServices::openUrl(url);
}
