/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "protocoladaptor.h"
#include "accountadaptor.h"
#include <qutim/event.h>
#include <QCryptographicHash>
#include <QDebug>

extern quint16 dbus_adaptor_event_id;

ProtocolAdaptor::ProtocolAdaptor(const QDBusConnection &dbus, Protocol *parent) :
		QDBusAbstractAdaptor(parent), m_dbus(dbus), m_protocol(parent)
{
	connect(parent, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), 
			this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	QString path = QLatin1String("/Protocol/");
	path += parent->id().replace('-', '_');
	m_path = QDBusObjectPath(path);
}

void ProtocolAdaptor::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	AccountAdaptor *adaptor = new AccountAdaptor(m_dbus, m_path, account);
	Event(dbus_adaptor_event_id, qVariantFromValue<Account*>(account)).send();
	m_dbus.registerObject(adaptor->path().path(), account, QDBusConnection::ExportAdaptors);
	m_accounts.insert(account->id(), adaptor->path());
	emit accountCreated(adaptor->path(), account->id());
}
