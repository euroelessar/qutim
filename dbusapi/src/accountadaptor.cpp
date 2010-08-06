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

#include "accountadaptor.h"
#include "chatunitadaptor.h"
#include <qutim/protocol.h>
#include <qutim/contact.h>
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
