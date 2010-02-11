/****************************************************************************
 *  icqprotocol.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "icqprotocol_p.h"
#include "icq_global.h"
#include "util.h"
#include <qutim/icon.h>
#include <qutim/statusactiongenerator.h>
#include "icqaccount.h"
#include <QStringList>
#include <QPointer>

namespace Icq
{

IcqProtocol *IcqProtocol::self = 0;

qutim_sdk_0_3::Status icqStatusToQutim(IcqStatus status)
{
	const IcqProtocolPrivate *d = IcqProtocol::instance()->d_func();
	if (status == IcqOffline)
		return d->statuses.value(IcqOffline);
	Status online = d->statuses.value(IcqOnline);
	if (status & IcqInvisible)
		return d->statuses.value(IcqInvisible, online);
	else /* if (status & IcqEvil)
		return d->statuses.value(IcqEvil, online);
	else if(status & IcqDepress)
		return d->statuses.value(IcqDepress, online);
	else if(status & IcqHome)
		return d->statuses.value(IcqHome, online);
	else if(status & IcqWork)
		return d->statuses.value(IcqWork, online);
	else if(status & IcqLunch)
		return d->statuses.value(IcqLunch, online);
	else */ if (status & IcqDND)
		return d->statuses.value(IcqDND, online);
	else if (status & IcqOccupied)
		return d->statuses.value(IcqOccupied, online);
	else if (status & IcqNA)
		return d->statuses.value(IcqNA, online);
	else if (status & IcqAway)
		return d->statuses.value(IcqAway, online);
	else if (status & IcqFFC)
		return d->statuses.value(IcqFFC, online);
	else
		return online;
}

IcqProtocol::IcqProtocol() :
	d_ptr(new IcqProtocolPrivate)
{
	Q_ASSERT(!self);
	Q_D(IcqProtocol);
	self = this;
	updateSettings();
	d->statuses.insert(IcqInvisible, Status(Status::Invisible));
	d->statuses.insert(IcqDND, Status(Status::DND));
	d->statuses.insert(IcqNA, Status(Status::NA));
	d->statuses.insert(IcqAway, Status(Status::Away));
	d->statuses.insert(IcqFFC, Status(Status::FreeChat));
	d->statuses.insert(IcqOnline, Status(Status::Online));
	d->statuses.insert(IcqOffline, Status(Status::Offline));
	QHash<IcqStatus, Status>::iterator itr = d->statuses.begin();
	QHash<IcqStatus, Status>::iterator endItr = d->statuses.end();
	while (itr != endItr) {
		itr->initIcon(QLatin1String("icq"));
		++itr;
	}
	{
		Status status(Status::DND);
		status.setSubtype(IcqOccupied);
		status.setIcon(Icon(QLatin1String("user-busy-occupied-icq")));
		status.setName(QT_TRANSLATE_NOOP("Status", "Busy"));
		d->statuses.insert(IcqOccupied, status);
	}
}

IcqProtocol::~IcqProtocol()
{
	self = 0;
}

void IcqProtocolPrivate::initActions()
{
	static bool inited = false;
	if (inited)
		return;
	foreach (Status status, statuses)
		MenuController::addAction(new StatusActionGenerator(status), &IcqAccount::staticMetaObject);
	inited = true;
}

void IcqProtocol::loadAccounts()
{
	Q_D(IcqProtocol);
	d->initActions();
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uin, accounts) {
		IcqAccount *acc = new IcqAccount(uin);
		d->accounts_hash->insert(uin, acc);
		emit accountCreated(acc);
	}

}

QList<Account *> IcqProtocol::accounts() const
{
	Q_D(const IcqProtocol);
	QList<Account *> accounts;
	QHash<QString, QPointer<IcqAccount> >::const_iterator it;
	for (it = d->accounts_hash->begin(); it != d->accounts_hash->end(); it++)
		accounts.append(it.value());
	return accounts;
}

Account *IcqProtocol::account(const QString &id) const
{
	Q_D(const IcqProtocol);
	return d->accounts_hash->value(id);
}

void IcqProtocol::updateSettings()
{
	Q_D(IcqProtocol);
	QString codecName = config("general").value("codec", "System").toString();
	Util::setAsciiCodec(QTextCodec::codecForName(codecName.toLatin1()));
	foreach (QPointer<IcqAccount> acc, *d->accounts_hash)
		acc->updateSettings();
}

void IcqProtocol::onStatusActionPressed()
{
	Q_D(IcqProtocol);
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	MenuController *item = action->data().value<MenuController *>();
	if (IcqAccount *account = qobject_cast<IcqAccount *>(item))
		account->setStatus(static_cast<Status::Type>(action->property("status").toInt()));
}

} // namespace Icq
