/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
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
	path += parent->id().replace('-', '_').replace(' ', '_');
	m_path = QDBusObjectPath(path);
	foreach (Account *account, parent->accounts())
		onAccountCreated(account);
}

void ProtocolAdaptor::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	AccountAdaptor *adaptor = new AccountAdaptor(m_dbus, m_path, account);
	Event(dbus_adaptor_event_id, qVariantFromValue<Account*>(account)).send();
	m_dbus.registerObject(adaptor->path().path(), account, QDBusConnection::ExportAdaptors);
	m_accounts.insert(account->id(), adaptor->path());
	emit accountCreated(adaptor->path(), account->id());
}

