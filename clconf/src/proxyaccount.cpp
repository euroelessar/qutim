/****************************************************************************
 *  proxyaccount.cpp
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

#include "proxyaccount.h"
#include "proxycontact.h"

ProxyAccount::ProxyAccount(Account *account) :
	Account(account->id(), account->protocol()), m_account(account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status)),
			SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)));
	connect(account, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
	connect(account, SIGNAL(nameChanged(QString)),
			SIGNAL(nameChanged(QString)));
}

QString ProxyAccount::name() const
{
	return m_account->name();
}

void ProxyAccount::setStatus(Status status)
{
	Account::setStatus(status);
	m_account->setStatus(status);
}

ChatUnit *ProxyAccount::getUnitForSession(ChatUnit *unit)
{
	if (ProxyContact *contact = qobject_cast<ProxyContact*>(unit))
		return contact->conference();
	return m_account->getUnitForSession(unit);
}

ChatUnit *ProxyAccount::getUnit(const QString &unitId, bool create)
{
	return m_account->getUnit(unitId, create);
}

void ProxyAccount::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account::setStatus(status);
	emit statusChanged(status);
}
