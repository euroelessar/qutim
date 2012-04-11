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
#include "jmessagesession_p.h"
#include "jmessagesession.h"
#include "jmessagehandler.h"
#include "../muc/jmucsession.h"
#include "jcontact.h"
#include "../../../jabber_global.h"
#include <qutim/chatsession.h>
#include <qutim/debug.h>
#include "../muc/jsessionconvertor.h"
#include "jcontactresource.h"

//using namespace gloox;
//using namespace qutim_sdk_0_3;

//namespace Jabber
//{
//inline qutim_sdk_0_3::ChatState gloox2qutIM(gloox::ChatStateType state)
//{
//	switch (state) {
//	case gloox::ChatStateActive:
//		return qutim_sdk_0_3::ChatStateActive;
//	case gloox::ChatStateInactive:
//		return qutim_sdk_0_3::ChatStateInActive;
//	case gloox::ChatStateGone:
//		return qutim_sdk_0_3::ChatStateGone;
//	case gloox::ChatStateComposing:
//		return qutim_sdk_0_3::ChatStateComposing;
//	case gloox::ChatStatePaused:
//		return qutim_sdk_0_3::ChatStatePaused;
//	default:
//		return qutim_sdk_0_3::ChatStateInActive;
//	}
//}

//inline gloox::ChatStateType qutIM2gloox(qutim_sdk_0_3::ChatState state)
//{
//	switch (state) {
//	case qutim_sdk_0_3::ChatStateActive:
//		return gloox::ChatStateActive;
//	case qutim_sdk_0_3::ChatStateInActive:
//		return gloox::ChatStateInactive;
//	case qutim_sdk_0_3::ChatStateGone:
//		return gloox::ChatStateGone;
//	case qutim_sdk_0_3::ChatStateComposing:
//		return gloox::ChatStateComposing;
//	case qutim_sdk_0_3::ChatStatePaused:
//		return gloox::ChatStatePaused;
//	default:
//		return gloox::ChatStateInvalid;
//	}
//}

////class JMessageReceiptFilter : public MessageFilter
////{
////public:
////	JMessageReceiptFilter(JMessageSession *s, MessageSession *p) :
////		MessageFilter(p), m_session(s), m_nextId(0) {}
////	void decorate(gloox::Message &msg);
////	void filter(gloox::Message &msg);
////	inline void setNextId(quint64 id) { m_nextId = id; }
////protected:
////	JMessageSession *m_session;
////	quint64 m_nextId;
////};

//struct ReceiptTrack
//{
//	inline ReceiptTrack(ChatSession *s, quint64 i) : session(s), id(i) {}
//	QPointer<ChatSession> session;
//	quint64 id;
//};
//typedef QHash<QByteArray, ReceiptTrack> ReceiptTrackMap;
//Q_GLOBAL_STATIC(ReceiptTrackMap, receiptTrackMap)

////void JMessageReceiptFilter::decorate(gloox::Message &msg)
////{
////	if (m_nextId > 0) {
////		ChatSession *session = 0;
////		session = ChatLayer::get(m_session->upperUnit(), false);
////		if (!session) {
////			if (ChatUnit *resource = m_session->upperUnit())
////				session = ChatLayer::get(resource->upperUnit(), false);
////		}
////		if (session)
////			receiptTrackMap()->insert(QByteArray(msg.id().data(), msg.id().size()),
////									  ReceiptTrack(session, m_nextId));
////		msg.addExtension(new Receipt(Receipt::Request));
////		m_nextId = 0;
////	}
////}

////void JMessageReceiptFilter::filter(gloox::Message &msg)
////{
////	if (const Receipt *receipt = msg.payload<Receipt>(ExtReceipt)) {
////		if (receipt->rcpt() == Receipt::Request) {
////			gloox::Message message(msg.subtype(), msg.from(), gloox::EmptyString,
////								   gloox::EmptyString, msg.thread());
////#if 0
////			// It's correct behaviour
////			Client *client = static_cast<JAccount*>(m_session->account())->client();
////			message.setID(client->getID());
////#else
////			// And it's legacy one, but compatible with qutIM 0.2 and other clients
////			message.setID(msg.id());
////#endif
////			message.addExtension(new Receipt(Receipt::Received, msg.id()));
////			send(message);
////			return;
////		}
////		ReceiptTrackMap *map = receiptTrackMap();
////		ReceiptTrackMap::iterator it = map->find(QByteArray(receipt->id().data(), receipt->id().size()));
////		if (it == map->end())
////			it = map->find(QByteArray(msg.id().data(), msg.id().size()));
////		if (it == map->end())
////			return;
////		if (receipt->rcpt() == Receipt::Received) {
////			const ReceiptTrack &track = it.value();
////			if (track.session) {
////				QEvent *ev = new qutim_sdk_0_3::MessageReceiptEvent(track.id, true);
////				qApp->postEvent(track.session, ev);
////			}
////			map->erase(it);
////			return;
////		}
////	}
////}

//JMessageSession::JMessageSession(JMessageHandler *handler, ChatUnit *unit, gloox::MessageSession *session)
//	: d_ptr(new JMessageSessionPrivate)
//{
//	Q_D(JMessageSession);
//	d->account = handler->account();
//	d->handler = handler;
//	d->session = session;
//	d->chatStateFilter = new ChatStateFilter(d->session);
//	//d->messageReceiptFilter = new JMessageReceiptFilter(this, d->session);
//	d->chatStateFilter->registerChatStateHandler(this);
//	d->session->registerMessageHandler(this);
//	d->handler->prepareMessageSession(this);
//	d->followChanges = session->threadID().empty();
//	d->unit = unit;
//	d->atDeathState = false;
//	d->id = QString::fromStdString(d_func()->session->threadID());
//	JMessageSessionOwner *owner = qobject_cast<JMessageSessionOwner*>(d->unit);
//	Q_ASSERT(owner);
//	owner->messageSessionCreated(this);
//}

//JMessageSession::JMessageSession(ChatUnit *unit) :
//	d_ptr(new JMessageSessionPrivate)
//{
//	Q_D(JMessageSession);
//	d->account = static_cast<JAccount *>(unit->account());
//	//d->handler = d->account->messageHandler();
//	//d->session = new MessageSession(d->account->client(), JID(unit->id().toStdString()));
//	//d->messageReceiptFilter = new JMessageReceiptFilter(this, d->session);
//	d->chatStateFilter = new ChatStateFilter(d->session);
//	d->chatStateFilter->registerChatStateHandler(this);
//	d->session->registerMessageHandler(this);
//	d->handler->prepareMessageSession(this);
//	d->followChanges = true;
//	d->unit = unit;
//	d->id = QString::fromStdString(d_func()->session->threadID());
//	JMessageSessionOwner *owner = qobject_cast<JMessageSessionOwner*>(d->unit);
//	Q_ASSERT(owner);
//	owner->messageSessionCreated(this);
//}

//JMessageSession::~JMessageSession()
//{
//	//		Q_D(JMessageSession);
//	//		if (d->account)
//	//			d->account->client()->disposeMessageSession(d->session);
//	//		if (d->handler)
//	//			d->handler->removeSessionId(d->id);
//}

//void JMessageSession::convertToMuc()
//{
//	Q_D(JMessageSession);
//	if (d->atDeathState)
//		return;
//	new JSessionConvertor(this);
//	//		JMUCSession *session = * new JMUCSession(this);
//	//		ChatLayer::get(this, true)->setChatUnit(session);
//	d->atDeathState = true;
//}

//QString JMessageSession::id() const
//{
//	return d_func()->id;
//}

//gloox::MessageSession *JMessageSession::session()
//{
//	return d_func()->session;
//}

//bool JMessageSession::sendMessage(const qutim_sdk_0_3::Message &message)
//{
//	Q_D(JMessageSession);

//	if (d->atDeathState || d->account->status() == Status::Offline)
//		return false;

//	d->messages << message;

//	//d->messageReceiptFilter->setNextId(message.id());
//	d->session->send(message.text().toStdString(), message.property("subject").toString().toStdString());
//	if (d->followChanges) {
//		d->handler->setSessionId(this, d->id);
//		d->followChanges = false;
//	}
//	return true;
//}

//void JMessageSession::handleMessage(const gloox::Message &msg, MessageSession *session)
//{
//	Q_D(JMessageSession);
//	Q_ASSERT((!session) || (d->session == session));
//	if (d->followChanges) {
//		d->handler->setSessionId(this, d->id);
//		d->followChanges = false;
//	}
//	qutim_sdk_0_3::Message coreMsg(QString::fromStdString(msg.body()));
//#if 1
//	// Workaround for a problem when messages from different resources of a contact
//	// are handled by one JMessageSession.
//	ChatUnit *sender = d->account->getUnit(QString::fromStdString(msg.from().full()), false);
//	coreMsg.setChatUnit(sender ? sender : d->unit.data());
//#else
//	coreMsg.setChatUnit(d->unit.data());
//#endif
//	coreMsg.setIncoming(true);
//	if (const DelayedDelivery *when = msg.when())
//		coreMsg.setTime(stamp2date(when->stamp()));
//	else
//		coreMsg.setTime(QDateTime::currentDateTime());
//	if (!msg.subject().empty())
//		coreMsg.setProperty("subject", QString::fromStdString(msg.subject()));
//	d->messages << coreMsg;
//	ChatLayer::get(d->unit, true)->appendMessage(coreMsg);
//}

//bool JMessageSession::event(QEvent *ev)
//{
//	if (ev->type() == ChatStateEvent::eventType()) {
//		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
//		d_func()->chatStateFilter->setChatState(qutIM2gloox(chatEvent->chatState()));
//		return true;
//	}
//	return QObject::event(ev);
//}

//void JMessageSession::handleChatState(const JID &from, gloox::ChatStateType state)
//{
//	Q_D(JMessageSession);
//	Q_UNUSED(from);
//	d->unit->setChatState(gloox2qutIM(state));
//}

//ChatUnit *JMessageSession::upperUnit()
//{
//	return d_func()->unit;
//}
//}

