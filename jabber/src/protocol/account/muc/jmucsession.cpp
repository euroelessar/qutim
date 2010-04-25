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
#include "../vcard/jvcardmanager.h"
#include "../../jprotocol.h"
#include "jbookmarkmanager.h"
#include <gloox/uniquemucroom.h>
#include <qutim/message.h>
#include <gloox/message.h>
#include <qutim/messagesession.h>
#include <gloox/vcardupdate.h>
#include "jabber_global.h"
#include <QStringBuilder>
#include <QMessageBox>
#include <QFile>
#include "jmucmanager.h"
#include "jconferenceconfig.h"

using namespace gloox;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	struct JMUCSessionPrivate
	{
		JAccount *account;
		gloox::MUCRoom *room;
		gloox::JID jid;
		QString nick;
		QString title;
		QHash<std::string, quint64> messages;
		QHash<QString, JMUCUser *> users;
		bool isJoined;
		int bookmarkIndex;
		bool isConfiguring;
		bool avatarsAutoLoad;
		bool isError;
		QDateTime lastMessage;
	};

	JMUCSession::JMUCSession(const JID &room, const QString &password, JAccount *account) : Conference(account), d_ptr(new JMUCSessionPrivate)
	{
		Q_D(JMUCSession);
		d->jid = room.bareJID();
		d->nick = QString::fromStdString(room.resource());
		d->room = new MUCRoom(account->client(), room, this, this);
		if (!password.isEmpty())
			d->room->setPassword(password.toStdString());
		d->account = account;
		d->isJoined = false;
		d->isConfiguring = false;
		d->isError = false;
		loadSettings();
	}

	JMUCSession::JMUCSession(JMessageSession *session) : Conference(session->account())
	{
		Q_ASSERT(!"Not yet implemented");
		Q_D(JMUCSession);
//		d->jid = JID();
		d->account = static_cast<JAccount *>(session->account());
		d->room = new UniqueMUCRoom(d->account->client(), EmptyString, this);
		join();
	}

	JMUCSession::~JMUCSession()
	{
		d_func()->room->leave();
	}

	qutim_sdk_0_3::Buddy *JMUCSession::me() const
	{
		Q_D(const JMUCSession);
		return d->users.value(d->nick);
	}

	ChatUnit *JMUCSession::participant(const QString &nick)
	{
		return d_func()->users.value(nick);
	}

	void JMUCSession::join()
	{
		Q_D(JMUCSession);
		Presence &pres = d->account->client()->presence();
		if (d->isJoined) {
			if (!ChatLayer::instance()->getSession(this, false))
				foreach (JMUCUser *muc, d->users.values())
					ChatLayer::get(this, true)->addContact(muc);
			d->room->setPresence(pres.subtype(), pres.status());
			d->users.value(d->nick)->setStatus(pres.subtype(), pres.priority());
		} else {
			foreach (JMUCUser *muc, d->users.values()) {
				ChatLayer::get(this, true)->removeContact(muc);
				delete muc;
			}
			d->users.clear();
			d->messages.clear();
			if (d->lastMessage.isValid())
				d->room->setRequestHistory(d->lastMessage.toUTC()
						.toString("yyyy-MM-ddThh:mm:ss.zzzZ").toStdString());
			d->room->join(pres.subtype(), pres.status(), pres.priority());
		}
	}

	void JMUCSession::leave()
	{
		d_func()->room->leave();
		d_func()->isJoined = false;
	}

	bool JMUCSession::isJoined()
	{
		return d_func()->isJoined;
	}

	QString JMUCSession::id() const
	{
		Q_D(const JMUCSession);
		return QString::fromStdString(d->room->name())
				% QLatin1Char('@')
				% QString::fromStdString(d->room->service());
	}

	void JMUCSession::sendMessage(const qutim_sdk_0_3::Message &message)
	{
		Q_D(JMUCSession);
		gloox::Message gMsg(gloox::Message::Groupchat, d->jid, message.text().toStdString());
		gMsg.setID(d->account->client()->getID());
		d->messages.insert(gMsg.id(), message.id());
		d->account->client()->send(gMsg);
	}

	void JMUCSession::handleMUCParticipantPresence(MUCRoom *room, const MUCRoomParticipant participant,
			const Presence &presence)
	{
		Q_D(JMUCSession);
		Q_ASSERT(room == d->room);
		QString nick = QString::fromStdString(participant.nick->resource());
		JMUCUser *user = d->users.value(nick, 0);
		if (!user && presence.subtype() != Presence::Unavailable) {
			user = new JMUCUser(this, nick);
			d->users.insert(nick, user);
			if (ChatSession *session = ChatLayer::instance()->getSession(this, false))
				session->addContact(user);
		} else if (!user) {
			return;
		} else if (presence.subtype() == Presence::Unavailable) {
			d->users.remove(nick);
			if (ChatSession *session = ChatLayer::instance()->getSession(this, false))
				session->removeContact(user);
		}
		user->setStatus(presence.presence(), presence.priority());
		if (presence.presence() != Presence::Unavailable && !presence.error()) {
			const VCardUpdate *vcard = presence.findExtension<VCardUpdate>(ExtVCardUpdate);
			if(vcard) {
				QString hash = QString::fromStdString(vcard->hash());
				if (user->avatarHash() != hash) {
					if(hash.isEmpty() || QFile(d->account->getAvatarPath()%QLatin1Char('/')%hash).exists())
						user->setAvatar(hash);
					else if (d->avatarsAutoLoad)
						d->account->connection()->vCardManager()->fetchVCard(user->id());
				}
			}
		}
		if (!d->isJoined && (presence.from().resource() == d->room->nick()))
			d->isJoined = true;
	}

	void JMUCSession::handleMUCMessage(MUCRoom *room, const gloox::Message &msg, bool priv)
	{
		Q_D(JMUCSession);
		Q_ASSERT(room == d->room);
		QString nick = QString::fromStdString(msg.from().resource());
		JMUCUser *user = d->users.value(nick, 0);
		if (!user)
			return;
		if (priv) {
			JMessageSession *session = qobject_cast<JMessageSession *>(d->account->getUnitForSession(user));
			if (!session) {
				MessageSession *glooxSession = new MessageSession(d->account->client(), msg.from(), false,
																  gloox::Message::Chat | gloox::Message::Normal);
				session = new JMessageSession(d->account->messageHandler(), user, glooxSession);
				d->account->messageHandler()->setSessionUnit(session, user);
				session->handleMessage(msg, glooxSession);
			}
		} else {
			d->lastMessage = QDateTime::currentDateTime();
			qutim_sdk_0_3::Message coreMsg(QString::fromStdString(msg.body()));
			coreMsg.setChatUnit(user);
			coreMsg.setIncoming(msg.from().resource() != d->room->nick());
			ChatSession *chatSession = ChatLayer::get(this, true);
			const DelayedDelivery *when = msg.when();
			if (when) {
				coreMsg.setProperty("history", true);
				coreMsg.setTime(stamp2date(when->stamp()));
			} else {
				coreMsg.setTime(d->lastMessage);
			}
			if (!coreMsg.isIncoming() && !when) {
				QHash<std::string, quint64>::iterator it = d->messages.find(msg.id());
				if (it != d->messages.end()) {
					qApp->postEvent(chatSession, new qutim_sdk_0_3::MessageReceiptEvent(it.value(), true));
					d->messages.erase(it);
				}
				return;
			}
			if (!msg.subject().empty())
				coreMsg.setProperty("subject", QString::fromStdString(msg.subject()));
			chatSession->appendMessage(coreMsg);
		}
	}

	bool JMUCSession::handleMUCRoomCreation(MUCRoom *room)
	{
		Q_ASSERT(room == d_func()->room);
		if(room) {
			showConfigDialog();
			return true;
		}
		return false;
	}

	void JMUCSession::handleMUCSubject(MUCRoom *room, const std::string &nick, const std::string &subject)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::handleMUCInviteDecline(MUCRoom *room, const JID &invitee, const std::string &reason)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::handleMUCError(MUCRoom *room, StanzaError error)
	{
		Q_D(JMUCSession);
		Q_ASSERT(room == d->room);
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
			QMessageBox::warning(0, tr("Join groupchat on")+" "+d->account->id(), text);
			d->account->conferenceManager()->leave(QString::fromStdString(d->jid.full()));
		}
		d->isError = true;
	}

	void JMUCSession::handleMUCInfo(MUCRoom *room, int features, const std::string &name, const DataForm *infoForm)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::handleMUCItems(MUCRoom *room, const Disco::ItemList &items)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::handleMUCConfigList(MUCRoom *room, const MUCListItemList &items, MUCOperation operation)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::handleMUCConfigForm(MUCRoom *room, const DataForm &form)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::handleMUCConfigResult(MUCRoom *room, bool success, MUCOperation operation)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::handleMUCRequest(MUCRoom *room, const DataForm &form)
	{
		Q_ASSERT(room == d_func()->room);
	}

	void JMUCSession::setBookmarkIndex(int index)
	{
		Q_D(JMUCSession);
		d->bookmarkIndex = index;
		if (index != -1)
			d->title = d->account->conferenceManager()->bookmarkManager()->bookmarks()[index].name;
		else
			d->title = id();
		emit titleChanged(d->title);
	}

	int JMUCSession::bookmarkIndex()
	{
		return d_func()->bookmarkIndex;
	}

	QString JMUCSession::title() const
	{
		return d_func()->title;
	}

	void JMUCSession::showConfigDialog()
	{
		d_func()->isConfiguring = true;
		JConferenceConfig *dialog = new JConferenceConfig(d_func()->room);
		connect(dialog, SIGNAL(destroyDialog()), SLOT(closeConfigDialog()));
		dialog->show();
	}

	void JMUCSession::closeConfigDialog()
	{
		d_func()->isConfiguring = false;
	}

	bool JMUCSession::enabledConfiguring()
	{
		return !d_func()->isConfiguring;
	}

	void JMUCSession::loadSettings()
	{
		d_func()->avatarsAutoLoad = JProtocol::instance()->config("general").value("getavatars", true);
	}

	void JMUCSession::clearSinceDate()
	{
		d_func()->lastMessage = QDateTime();
	}

	bool JMUCSession::isError()
	{
		return d_func()->isError;
	}

	ChatUnitList JMUCSession::lowerUnits()
	{
		ChatUnitList list;
		foreach(ChatUnit *unit, d_func()->users)
			list << unit;
		return list;
	}
}
