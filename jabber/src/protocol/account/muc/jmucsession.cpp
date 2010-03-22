/****************************************************************************
 *  jmucsession.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "jmucsession.h"
#include "jmucuser.h"
#include "../jaccount.h"
#include "../roster/jmessagesession.h"
#include "../roster/jmessagehandler.h"
#include <gloox/uniquemucroom.h>
#include <qutim/message.h>
#include <gloox/message.h>
#include <qutim/messagesession.h>
#include "jabber_global.h"
#include <QStringBuilder>
#include <QMessageBox>
#include "jmucmanager.h"
#include "jconferenceconfig.h"

using namespace gloox;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	JMUCSession::JMUCSession(const JID &room, const QString &password, JAccount *account) : Conference(account)
	{
		m_roomJid = room.bareJID();
		m_nick = QString::fromStdString(room.resource());
		m_room = new MUCRoom(account->client(), room, this, this);
		if (!password.isEmpty())
			m_room->setPassword(password.toStdString());
		m_account = account;
		m_isJoined = false;
		m_isConfiguring = false;
	}

	JMUCSession::JMUCSession(JMessageSession *session) : Conference(session->account())
	{
		Q_ASSERT(!"Not yet implemented");
//		m_roomJid = JID();
		m_account = static_cast<JAccount *>(session->account());
		m_room = new UniqueMUCRoom(m_account->client(), EmptyString, this);
		join();
	}

	JMUCSession::~JMUCSession()
	{
		m_room->leave();
	}

	qutim_sdk_0_3::Buddy *JMUCSession::me() const
	{
		return m_users.value(m_nick);
	}

	void JMUCSession::join()
	{
		Presence &pres = m_account->client()->presence();
		m_room->join(pres.subtype(), pres.status(), pres.priority());
	}

	void JMUCSession::leave()
	{
		m_room->leave();
	}

	QString JMUCSession::id() const
	{
		return QString::fromStdString(m_room->name())
				% QLatin1Char('@')
				% QString::fromStdString(m_room->service());
	}

	void JMUCSession::sendMessage(const qutim_sdk_0_3::Message &message)
	{
		gloox::Message gMsg(gloox::Message::Groupchat, m_roomJid, message.text().toStdString());
		gMsg.setID(m_account->client()->getID());
		m_messages.insert(gMsg.id(), message.id());
		m_account->client()->send(gMsg);
	}

	void JMUCSession::handleMUCParticipantPresence(MUCRoom *room, const MUCRoomParticipant participant,
												   const Presence &presence)
	{
		Q_ASSERT(room == m_room);

		QString nick = QString::fromStdString(participant.nick->resource());
		JMUCUser *user = m_users.value(nick, 0);
		if (!user && presence.subtype() != Presence::Unavailable) {
			user = new JMUCUser(this, nick);
			m_users.insert(nick, user);
			ChatLayer::get(this, true)->addContact(user);
		} else if (!user) {
			return;
		} else if (presence.subtype() == Presence::Unavailable) {
			m_users.remove(nick);
			ChatLayer::get(this, true)->removeContact(user);
		}

		if (!m_isJoined && (presence.from().resource() == m_room->nick()))
			m_isJoined = true;
	}

	void JMUCSession::handleMUCMessage(MUCRoom *room, const gloox::Message &msg, bool priv)
	{
		Q_ASSERT(room == m_room);
		QString nick = QString::fromStdString(msg.from().resource());
		JMUCUser *user = m_users.value(nick, 0);
		if (!user)
			return;
		if (priv) {
			JMessageSession *session = qobject_cast<JMessageSession *>(m_account->getUnitForSession(user));
			if (!session) {
				MessageSession *glooxSession = new MessageSession(m_account->client(), msg.from(), false,
																  gloox::Message::Chat | gloox::Message::Normal);
				session = new JMessageSession(m_account->messageHandler(), user, glooxSession);
				m_account->messageHandler()->setSessionUnit(session, user);
				session->handleMessage(msg, glooxSession);
			}
		} else {
			qutim_sdk_0_3::Message coreMsg(QString::fromStdString(msg.body()));
			coreMsg.setChatUnit(user);
			coreMsg.setIncoming(msg.from().resource() != m_room->nick());
			ChatSession *chatSession = ChatLayer::get(this, true);
			if (!coreMsg.isIncoming()) {
				QHash<std::string, quint64>::iterator it = m_messages.find(msg.id());
				if (it != m_messages.end()) {
					qApp->postEvent(chatSession, new qutim_sdk_0_3::MessageReceiptEvent(it.value(), true));
					m_messages.erase(it);
				}
				return;
			}
			if (const DelayedDelivery *when = msg.when())
				coreMsg.setTime(stamp2date(when->stamp()));
			if (!msg.subject().empty())
				coreMsg.setProperty("subject", QString::fromStdString(msg.subject()));
			chatSession->appendMessage(coreMsg);
		}
	}

	bool JMUCSession::handleMUCRoomCreation(MUCRoom *room)
	{
		Q_ASSERT(room == m_room);
		return true;
	}

	void JMUCSession::handleMUCSubject(MUCRoom *room, const std::string &nick, const std::string &subject)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::handleMUCInviteDecline(MUCRoom *room, const JID &invitee, const std::string &reason)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::handleMUCError(MUCRoom *room, StanzaError error)
	{
		Q_ASSERT(room == m_room);
		bool nnr = false;
		QString text;
		switch(error) {
		case StanzaErrorNotAuthorized:
			text=tr("Not authorized: Password required.");
			break;
		case StanzaErrorForbidden:
			text=tr("Forbidden: Access denied, user is banned.");
			break;
		case StanzaErrorItemNotFound:
			text=tr("Item not found: The room does not exist.");
			break;
		case StanzaErrorNotAllowed:
			text=tr("Not allowed: Room creation is restricted.");
			break;
		case StanzaErrorNotAcceptable :
			text=tr("Not acceptable: Room nicks are locked down.");
			break;
		case StanzaErrorRegistrationRequired:
			text=tr("Registration required: User is not on the member list.");
			break;
		case StanzaErrorConflict:
			text=tr("Conflict: Desired room nickname is in use or registered by another user.");
			break;
		case StanzaErrorServiceUnavailable:
			text=tr("Service unavailable: Maximum number of users has been reached.");
			break;
		default:
			nnr = true;
			text=tr("Unknown error: No description.");
			break;
		}
		if (nnr) {

		} else {
			QMessageBox::warning(0, tr("Join groupchat on")+" "+m_account->id(), text);
			m_account->conferenceManager()->leave(QString::fromStdString(m_roomJid.full()));
		}
	}

	void JMUCSession::handleMUCInfo(MUCRoom *room, int features, const std::string &name, const DataForm *infoForm)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::handleMUCItems(MUCRoom *room, const Disco::ItemList &items)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::handleMUCConfigList(MUCRoom *room, const MUCListItemList &items, MUCOperation operation)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::handleMUCConfigForm(MUCRoom *room, const DataForm &form)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::handleMUCConfigResult(MUCRoom *room, bool success, MUCOperation operation)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::handleMUCRequest(MUCRoom *room, const DataForm &form)
	{
		Q_ASSERT(room == m_room);
	}

	void JMUCSession::setBookmarkIndex(int index)
	{
		m_bookmarkIndex = index;
	}

	int JMUCSession::bookmarkIndex()
	{
		return m_bookmarkIndex;
	}

	void JMUCSession::showConfigDialog()
	{
		m_isConfiguring = true;
		JConferenceConfig *dialog = new JConferenceConfig(m_room);
		connect(dialog, SIGNAL(destroyDialog()), SLOT(closeConfigDialog()));
		dialog->show();
	}

	void JMUCSession::closeConfigDialog()
	{
		m_isConfiguring = false;
	}

	bool JMUCSession::enabledConfiguring()
	{
		return !m_isConfiguring;
	}
}
