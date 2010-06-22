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

#include "chatlayeradapter.h"
#include "chatsessionadapter.h"
#include <qutim/account.h>

ChatLayerAdapter::ChatLayerAdapter(const QDBusConnection &dbus) :
		QDBusAbstractAdaptor(ChatLayer::instance()), m_dbus(dbus)
{
	QList<QDBusObjectPath> list = sessions();
	Q_UNUSED(list);
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

QDBusObjectPath ChatLayerAdapter::session(const QDBusObjectPath &chatUnit, bool create)
{
	QObject *obj = m_dbus.objectRegisteredAt(chatUnit.path());
	ChatSession *session = obj ? ChatLayer::instance()->getSession(obj, create) : 0;
	QDBusObjectPath path = ChatSessionAdapter::ensurePath(m_dbus, session);
	return path;
}

QDBusObjectPath ChatLayerAdapter::session(const QDBusObjectPath &accountPath,
										  const QString &id, bool create)
{
	QObject *obj = m_dbus.objectRegisteredAt(accountPath.path());
	Account *account = qobject_cast<Account*>(obj);
	ChatSession *session = ChatLayer::instance()->getSession(account, id, create);
	QDBusObjectPath path = ChatSessionAdapter::ensurePath(m_dbus, session);
	return path;
}

QList<QDBusObjectPath> ChatLayerAdapter::sessions() const
{
	QList<QDBusObjectPath> list;
	foreach (ChatSession *session, ChatLayer::instance()->sessions())
		list << ChatSessionAdapter::ensurePath(m_dbus, session);
	return list;
}

void ChatLayerAdapter::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	QDBusObjectPath path = ChatSessionAdapter::ensurePath(m_dbus, session);
	emit sessionCreated(path);
}
