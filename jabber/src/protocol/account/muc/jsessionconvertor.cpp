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

#include "jsessionconvertor.h"
#include "../roster/jmessagesession_p.h"
#include "jmucmanager.h"
#include "jmucsession.h"
#include "jabber_global.h"
#include <qutim/messagesession.h>
#include <gloox/message.h>
#include <gloox/uniquemucroom.h>
#include <QStringBuilder>

using namespace gloox;

namespace Jabber
{
	class JSessionConvertorPrivate
	{
	public:
		JAccount *account;
		JMessageSession *session;
		gloox::UniqueMUCRoom *room;
		int serverId;
		QMap<int, JDiscoItem> items;
	};

	JSessionConvertor::JSessionConvertor(JMessageSession *session) :
			QObject(session), d_ptr(new JSessionConvertorPrivate)
	{
		Q_D(JSessionConvertor);
		d->session = session;
		d->account = d->session->d_func()->account;
		d->room = 0;
		JDiscoItem item;
		item.setJID(QString::fromStdString(d->account->client()->jid().server()));
		d->serverId = d->account->discoManager()->getItems(this, item);
	}
	
	JSessionConvertor::~JSessionConvertor()
	{
	}
	
	void JSessionConvertor::setInfo(int id)
	{
		Q_D(JSessionConvertor);
		QMap<int, JDiscoItem>::iterator it = d->items.find(id);
		if (it == d->items.end())
			return;
		JDiscoItem item = it.value();
		d->items.erase(it);
		if (!item.hasFeature(QLatin1String("http://jabber.org/protocol/muc")))
			return;
		d->items.clear();
		processServer(item.jid());
	}

	void JSessionConvertor::setItems(int id, const QList<JDiscoItem> &items)
	{
		Q_D(JSessionConvertor);
		if(id != d->serverId)
			return;
		
		foreach (const JDiscoItem &item, items) {
			int itemId = d->account->discoManager()->getInfo(this, item);
			d->items.insert(itemId, item);
		}
	}

	void JSessionConvertor::setError(int id)
	{
		Q_D(JSessionConvertor);
		if (id == d->serverId)
			processServer(QLatin1String("conference.jabber.org"));
		else
			d->items.remove(id);
	}
	
	void JSessionConvertor::handleMUCParticipantPresence(MUCRoom *room,
														 const MUCRoomParticipant participant,
														 const Presence &presence)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		if (presence.from().resource() == d->room->nick()) {
			JID jid = presence.from().bareJID();
			JID inJid = d->session->upperUnit()->id().toStdString();
			const JID &outJid = d->account->client()->jid();
			std::string thread = d->session->d_func()->session->threadID();
			JMUCSession *session = new JMUCSession(d->account, d->room, thread);
			ChatLayer::get(d->session, true)->setChatUnit(session);
			foreach (const qutim_sdk_0_3::Message &msg, d->session->d_func()->messages) {
				gloox::Message gMsg(gloox::Message::Groupchat, jid, msg.text().toStdString(),
									gloox::EmptyString, thread);
				gMsg.addExtension(new DelayedDelivery(msg.isIncoming() ? inJid : outJid,
													  date2stamp(msg.time())));
				d->account->client()->send(gMsg);
			}
			d->room->invite(d->session->upperUnit()->id().toStdString(), gloox::EmptyString, thread);
			session->handleMUCParticipantPresence(room, participant, presence);
		}
		d->session->deleteLater();
		Q_UNUSED(participant);
		Q_UNUSED(presence);
	}

	void JSessionConvertor::handleMUCMessage(MUCRoom *room, const gloox::Message &msg, bool priv)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		Q_UNUSED(msg);
		Q_UNUSED(priv);
	}

	bool JSessionConvertor::handleMUCRoomCreation(MUCRoom *room)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		return true;
	}

	void JSessionConvertor::handleMUCSubject(MUCRoom *room, const std::string &nick,
											 const std::string &subject)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		Q_UNUSED(nick);
		Q_UNUSED(subject);
	}

	void JSessionConvertor::handleMUCInviteDecline(MUCRoom *room, const JID &invitee,
												   const std::string &reason)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		Q_UNUSED(invitee);
		Q_UNUSED(reason);
	}

	void JSessionConvertor::handleMUCError(MUCRoom *room, StanzaError error)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		Q_UNUSED(error);
	}

	void JSessionConvertor::handleMUCInfo(MUCRoom *room, int features, const std::string &name,
										  const DataForm *infoForm)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		Q_UNUSED(features);
		Q_UNUSED(name);
		Q_UNUSED(infoForm);
	}

	void JSessionConvertor::handleMUCItems(MUCRoom *room, const Disco::ItemList &items)
	{
		Q_D(JSessionConvertor);
		Q_ASSERT(d->room == room);
		Q_UNUSED(items);
	}
	
	void JSessionConvertor::processServer(const QString &server)
	{
		Q_D(JSessionConvertor);
		QString jid = server % QLatin1Char('/') % d->account->name();
		d->room = new UniqueMUCRoom(d->account->client(), jid.toStdString(), this);
		d->room->join();
	}
}
