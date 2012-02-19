/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "oscarchat.h"
#include "icqaccount.h"
#include "icqcontact.h"
#include <qutim/chatsession.h>

namespace qutim_sdk_0_3 {
namespace oscar {

OscarChat::OscarChat(const RoomId &id, ChatNavigation *navigation)
    : Conference(navigation->account()), m_roomId(id), m_navigation(navigation)
{
	m_id = QLatin1String(id.cookie);
	navigation->account()->addChat(id.cookie, this);
}

OscarChat::~OscarChat()
{
	m_navigation->account()->removeChat(m_roomId.cookie);
}

QString OscarChat::id() const
{
	return m_id;
}

QString OscarChat::title() const
{
	return m_connection ? m_connection.data()->title() : QString();
}

Buddy *OscarChat::me() const
{
	return m_navigation->account()->getContact(account()->id(), true);
}

void OscarChat::attach(ChatConnection *connection)
{
	if (m_connection.data() == connection)
		return;
	debug() << Q_FUNC_INFO;
	delete m_connection.data();
	m_connection = connection;
	connect(connection, SIGNAL(usersJoined(QList<qutim_sdk_0_3::oscar::IcqContact*>)),
	        SLOT(onUsersJoined(QList<qutim_sdk_0_3::oscar::IcqContact*>)));
	connect(connection, SIGNAL(usersLeaved(QList<qutim_sdk_0_3::oscar::IcqContact*>)),
	        SLOT(onUsersLeaved(QList<qutim_sdk_0_3::oscar::IcqContact*>)));
	connect(connection, SIGNAL(titleChanged(QString,QString)),
	        SIGNAL(titleChanged(QString,QString)));
	foreach (Buddy *buddy, connection->participants())
		ChatLayer::get(this, true)->addContact(buddy);
	onUsersJoined(connection->participants());
	if (!isJoined())
		setJoined(true);
}

bool OscarChat::sendMessage(const qutim_sdk_0_3::Message &message)
{
	if (!m_connection)
		return false;
	m_connection.data()->sendMessage(message);
	return true;
}

QList<ChatUnit*> OscarChat::lowerUnits()
{
	QList<ChatUnit*> units;
	if (!m_connection)
		return units;
	foreach (ChatUnit *unit, m_connection.data()->participants())
		units << unit;
	return units;
}

void OscarChat::doJoin()
{
	if (m_connection)
		return;
	m_navigation->joinRoom(m_roomId);
}

void OscarChat::doLeave()
{
	if (!m_connection)
		return;
	m_connection.data()->disconnectFromHost();
	m_connection.clear();
	setJoined(false);
}

void OscarChat::invite(qutim_sdk_0_3::Contact *contact, const QString &reason)
{
	if (IcqContact *icqContact = qobject_cast<IcqContact*>(contact))
		m_connection.data()->inviteUser(icqContact, reason);
}

void OscarChat::onUsersJoined(const QList<qutim_sdk_0_3::oscar::IcqContact*> &contacts)
{
	for (int i = 0; i < contacts.size(); ++i)
		emit lowerUnitAdded(contacts.at(i));
}

void OscarChat::onUsersLeaved(const QList<qutim_sdk_0_3::oscar::IcqContact*> &contacts)
{
	Q_UNUSED(contacts);
//	for (int i = 0; i < contacts.size(); ++i)
//		emit lowerUnitRemoved(contacts.at(i));
}

}
}
