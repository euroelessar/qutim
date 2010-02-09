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
#include "icqaccount.h"
#include <QStringList>
#include <QPointer>

namespace Icq
{

IcqProtocol *IcqProtocol::self = 0;


// May be there should be qutIM global hash in libqutim?..
qutim_sdk_0_3::Status icqStatusToQutim(quint16 status)
{
	if (status & IcqFlagInvisible) {
		static Status *status = NULL;
		if (!status) {
			status = new Status(Status::Invisible);
			status->initIcon(QLatin1String("icq"));
		}
		return *status;
	} else /*if(status & IcqFlagEvil)
		return Evil;
	else if(status & IcqFlagDepress)
		return Depression;
	else if(status & IcqFlagHome)
		return AtHome;
	else if(status & IcqFlagWork)
		return AtWork;
	else if(status & IcqFlagLunch)
		return OutToLunch;
	else */if (status & IcqFlagDND) {
		static Status *status = NULL;
		if (!status) {
			status = new Status(Status::DND);
			status->initIcon(QLatin1String("icq"));
		}
		return *status;
	} else if (status & IcqFlagOccupied) {
		static Status *status = NULL;
		if (!status) {
			status = new Status(Status::DND);
			status->setSubtype(IcqFlagOccupied);
			status->setIcon(Icon(QLatin1String("user-busy-occupied-icq")));
		}
		return *status;
	} else if (status & IcqFlagNA) {
		static Status *status = NULL;
		if (!status) {
			status = new Status(Status::NA);
			status->initIcon(QLatin1String("icq"));
		}
		return *status;
	} else if (status & IcqFlagAway) {
		static Status *status = NULL;
		if (!status) {
			status = new Status(Status::Away);
			status->initIcon(QLatin1String("icq"));
		}
		return *status;
	} else if (status & IcqFlagFFC) {
		static Status *status = NULL;
		if (!status) {
			status = new Status(Status::FreeChat);
			status->initIcon(QLatin1String("icq"));
		}
		return *status;
	} else {
		static Status *status = NULL;
		if (!status) {
			status = new Status(Status::Online);
			status->initIcon(QLatin1String("icq"));
		}
		return *status;
	}
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

void initActions(IcqProtocol *proto)
{
	static bool inited = false;
	if (inited)
		return;
	QList<ActionGenerator *> actions;
	actions << (new ActionGenerator(Icon("user-online-icq"),
				LocalizedString("Status", "Online"), proto,
				SLOT(onStatusActionPressed())))->addProperty("status", Status::Online)->setPriority(Status::Online);
	actions << (new ActionGenerator(Icon("user-online-chat-icq"),
				LocalizedString("Status", "Free for chat"), proto,
				SLOT(onStatusActionPressed())))->addProperty("status", Status::FreeChat)->setPriority(Status::FreeChat);
	actions << (new ActionGenerator(Icon("user-away-icq"),
				LocalizedString("Status", "Away"), proto,
				SLOT(onStatusActionPressed())))->addProperty("status", Status::Away)->setPriority(Status::Away);
	actions << (new ActionGenerator(Icon("user-away-extended-icq"),
				LocalizedString("Status", "NA"), proto,
				SLOT(onStatusActionPressed())))->addProperty("status", Status::NA)->setPriority(Status::NA);
	actions << (new ActionGenerator(Icon("user-busy-icq"),
				LocalizedString("Status", "DND"), proto,
				SLOT(onStatusActionPressed())))->addProperty("status", Status::DND)->setPriority(Status::DND);
	actions << (new ActionGenerator(Icon("user-offline-icq"),
				LocalizedString("Status", "Offline"), proto,
				SLOT(onStatusActionPressed())))->addProperty("status", Status::Offline)->setPriority(Status::Offline);
	foreach (ActionGenerator *action, actions)
		MenuController::addAction(action, &IcqAccount::staticMetaObject);
	inited = true;
}

void IcqProtocol::loadAccounts()
{
	Q_D(IcqProtocol);
	initActions(this);
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uin, accounts)
		d->accounts_hash->insert(uin, new IcqAccount(uin));
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
