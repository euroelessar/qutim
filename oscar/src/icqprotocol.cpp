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

typedef QHash<IcqStatus, Status> StatusList;
static void init_status_list(StatusList &list)
{
	list.insert(IcqInvisible, Status(Status::Invisible));
	list.insert(IcqDND, Status(Status::DND));
	list.insert(IcqNA, Status(Status::NA));
	list.insert(IcqAway, Status(Status::Away));
	list.insert(IcqFFC, Status(Status::FreeChat));
	list.insert(IcqOnline, Status(Status::Online));
	list.insert(IcqOffline, Status(Status::Offline));
	QHash<IcqStatus, Status>::iterator itr = list.begin();
	QHash<IcqStatus, Status>::iterator endItr = list.end();
	while (itr != endItr) {
		itr->initIcon(QLatin1String("icq"));
		++itr;
	}
	{
		Status status(Status::DND);
		status.setSubtype(IcqOccupied);
		status.setIcon(Icon(QLatin1String("user-busy-occupied-icq")));
		status.setName(QT_TRANSLATE_NOOP("Status", "Busy"));
		list.insert(IcqOccupied, status);
	}
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(StatusList, statusList, init_status_list(*x));

qutim_sdk_0_3::Status icqStatusToQutim(IcqStatus status)
{
	const StatusList &list = *statusList();
	if (status == IcqOffline)
		return list.value(IcqOffline);
	Status online = list.value(IcqOnline);
	if (status & IcqInvisible)
		return list.value(IcqInvisible, online);
	else /* if (status & IcqEvil)
		return list.value(IcqEvil, online);
	else if(status & IcqDepress)
		return list.value(IcqDepress, online);
	else if(status & IcqHome)
		return list.value(IcqHome, online);
	else if(status & IcqWork)
		return list.value(IcqWork, online);
	else if(status & IcqLunch)
		return list.value(IcqLunch, online);
	else */ if (status & IcqDND)
		return list.value(IcqDND, online);
	else if (status & IcqOccupied)
		return list.value(IcqOccupied, online);
	else if (status & IcqNA)
		return list.value(IcqNA, online);
	else if (status & IcqAway)
		return list.value(IcqAway, online);
	else if (status & IcqFFC)
		return list.value(IcqFFC, online);
	else
		return online;
}

IcqProtocol::IcqProtocol() :
	d_ptr(new IcqProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
	updateSettings();
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
	foreach (Status status, *statusList())
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
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	MenuController *item = action->data().value<MenuController *>();
	if (IcqAccount *account = qobject_cast<IcqAccount *>(item))
		account->setStatus(static_cast<Status::Type>(action->property("status").toInt()));
}

} // namespace Icq
