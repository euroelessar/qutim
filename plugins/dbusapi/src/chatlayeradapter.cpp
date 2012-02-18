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

