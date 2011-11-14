/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "accountadaptor.h"
#include "chatunitadaptor.h"
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/conference.h>
#include <QCryptographicHash>
#include <QDBusError>
#include <QDebug>

Q_GLOBAL_STATIC(AccountPathHash, accountHash)

const AccountPathHash &AccountAdaptor::hash()
{
	return *accountHash();
}

AccountAdaptor::AccountAdaptor(const QDBusConnection &dbus, const QDBusObjectPath &proto, Account *account) :
		QDBusAbstractAdaptor(account), m_dbus(dbus), m_account(account), m_protocolPath(proto)
{
	QString path = "/Account/";
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(account->protocol()->id().toUtf8());
	hash.addData("\0", 1);
	hash.addData(account->id().toUtf8());
	path += QLatin1String(hash.result().toHex());
	m_path = QDBusObjectPath(path);
	connect(account, SIGNAL(nameChanged(QString,QString)),
			this, SIGNAL(nameChanged(QString,QString)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(onContactCreated(qutim_sdk_0_3::Contact*)));
	connect(account, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			this, SLOT(onConferenceCreated(qutim_sdk_0_3::Conference*)));
	accountHash()->insert(account, m_path);
	foreach (Contact *contact, qFindChildren<Contact*>(account))
		ChatUnitAdaptor::ensurePath(m_dbus, contact);
}

AccountAdaptor::~AccountAdaptor()
{
	accountHash()->remove(m_account);
}

QDBusObjectPath AccountAdaptor::chatUnit(const QString &unitId, bool create)
{
	ChatUnit *unit = m_account->getUnit(unitId, create);
	return ChatUnitAdaptor::ensurePath(m_dbus, unit);
}

QStringList AccountAdaptor::contacts() const
{
	QStringList list;
	foreach (Contact *contact, qFindChildren<Contact*>(m_account))
		list << contact->id();
	return list;
}

void AccountAdaptor::onContactCreated(Contact *contact)
{
	QDBusObjectPath path = ChatUnitAdaptor::ensurePath(m_dbus, contact);
	emit contactCreated(path, contact->id());
}

void AccountAdaptor::onConferenceCreated(Conference *conference)
{
	QDBusObjectPath path = ChatUnitAdaptor::ensurePath(m_dbus, conference);
	emit contactCreated(path, conference->id());
}

