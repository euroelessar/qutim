/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "webkitpreviewunits_p.h"

WebKitPreviewChatUnit::WebKitPreviewChatUnit(const QVariantMap &data, qutim_sdk_0_3::Account *account)
    : ChatUnit(account), m_data(data)
{
}

WebKitPreviewChatUnit::~WebKitPreviewChatUnit()
{
}

QString WebKitPreviewChatUnit::id() const
{
	return m_data.value(QLatin1String("UID")).toString();
}

QString WebKitPreviewChatUnit::title() const
{
	return m_data.value(QLatin1String("Display Name")).toString();
}

QString WebKitPreviewChatUnit::avatar() const
{
	return m_data.value(QLatin1String("UserIcon Name")).toString();
}

bool WebKitPreviewChatUnit::sendMessage(const qutim_sdk_0_3::Message &message)
{
}

WebKitPreviewAccount::WebKitPreviewAccount(const QVariantMap &data, qutim_sdk_0_3::Protocol *protocol)
    : Account(data.value(QLatin1String("UID")).toString(), protocol), m_data(data)
{
}

WebKitPreviewAccount::~WebKitPreviewAccount()
{
}

QString WebKitPreviewAccount::name() const
{
	return m_data.value(QLatin1String("Display Name")).toString();
}

QString WebKitPreviewAccount::avatar() const
{
	return m_data.value(QLatin1String("UserIcon Name")).toString();
}

qutim_sdk_0_3::ChatUnit *WebKitPreviewAccount::getUnit(const QString &unitId, bool create)
{
	Q_UNUSED(unitId);
	Q_UNUSED(create);
	return 0;
}

WebKitPreviewProtocol::WebKitPreviewProtocol(QObject *parent)
{
	setParent(parent);
}

WebKitPreviewProtocol::~WebKitPreviewProtocol()
{
}

QList<qutim_sdk_0_3::Account*> WebKitPreviewProtocol::accounts() const
{
	return QList<qutim_sdk_0_3::Account*>();
}

qutim_sdk_0_3::Account *WebKitPreviewProtocol::account(const QString &id) const
{
	Q_UNUSED(id);
	return 0;
}

void WebKitPreviewProtocol::loadAccounts()
{
}

WebKitPreviewSession::WebKitPreviewSession() : ChatSession(0)
{
}

WebKitPreviewSession::~WebKitPreviewSession()
{
}

qutim_sdk_0_3::ChatUnit *WebKitPreviewSession::getUnit() const
{
	return m_unit;
}

void WebKitPreviewSession::setChatUnit(qutim_sdk_0_3::ChatUnit *unit)
{
	m_unit = unit;
}

QTextDocument *WebKitPreviewSession::getInputField()
{
	return 0;
}

void WebKitPreviewSession::markRead(quint64 id)
{
	Q_UNUSED(id);
}

qutim_sdk_0_3::MessageList WebKitPreviewSession::unread() const
{
	return qutim_sdk_0_3::MessageList();
}

void WebKitPreviewSession::addContact(qutim_sdk_0_3::Buddy *c)
{
	Q_UNUSED(c);
}

void WebKitPreviewSession::removeContact(qutim_sdk_0_3::Buddy *c)
{
	Q_UNUSED(c);
}

void WebKitPreviewSession::doSetActive(bool active)
{
	Q_UNUSED(active);
}

qint64 WebKitPreviewSession::doAppendMessage(qutim_sdk_0_3::Message &message)
{
	return message.id();
}

