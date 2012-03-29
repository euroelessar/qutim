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
#include "jmessagehandler.h"
#include "jmessagesession.h"
#include "../jaccount.h"
#include "jcontact.h"
#include "jcontactresource.h"
#include "../muc/jmucuser.h"
#include "../muc/jmucmanager.h"
#include "../../../sdk/jabber.h"
#include <qutim/debug.h>
#include <jreen/receipt.h>
#include <jreen/client.h>
#include <jreen/chatstate.h>
#include <jreen/error.h>
#include "jroster.h"
#include <QApplication>

namespace Jabber
{
//Jreen

JMessageSessionHandler::JMessageSessionHandler(JAccount *account) :
	m_account(account)
{

}

JMessageSessionHandler::~JMessageSessionHandler()
{
}

void JMessageSessionHandler::handleMessageSession(Jreen::MessageSession *session)
{
	Q_ASSERT(session);
	session->registerMessageFilter(new JMessageReceiptFilter(m_account,session));
	QObject::connect(session, SIGNAL(messageReceived(Jreen::Message)),
					 m_account->roster(), SLOT(onNewMessage(Jreen::Message)));
}

class JMessageSessionManagerPrivate
{
public:
	JMessageSessionManagerPrivate(JMessageSessionManager *q) : q_ptr(q) {}
	JMessageSessionManager *q_ptr;
	JAccount *account;
};

JMessageSessionManager::JMessageSessionManager(JAccount *account) :
	Jreen::MessageSessionManager(account->client()),
	d_ptr(new JMessageSessionManagerPrivate(this))
{
	Q_D(JMessageSessionManager);
	d->account = account;
	QList<Jreen::Message::Type> types;
	types.append(Jreen::Message::Chat);
	types.append(Jreen::Message::Headline);
	types.append(Jreen::Message::Normal);
	types.append(Jreen::Message::Invalid);

	registerMessageSessionHandler(new JMessageSessionHandler(account),types);
}

JMessageSessionManager::~JMessageSessionManager()
{

}

void JMessageSessionManager::handleMessage(const Jreen::Message &message)
{
	debug() << "handle message";
	return Jreen::MessageSessionManager::handleMessage(message);
}

void JMessageSessionManager::sendMessage(qutim_sdk_0_3::ChatUnit *unit, const qutim_sdk_0_3::Message &message)
{
	JID jid = unit->id();
	Jreen::MessageSession *s = session(jid, Jreen::Message::Chat, true);

	Jreen::Message msg(Jreen::Message::Chat,
					   jid,
					   message.text(),
					   message.property("subject").toString());
	msg.setID(QString::number(message.id()));
	s->sendMessage(msg);
	//We will close the session at Jreen together with a session in qutim
	s->setParent(ChatLayer::get(const_cast<ChatUnit*>(message.chatUnit()),true));
}

JMessageReceiptFilter::JMessageReceiptFilter(JAccount *account,
											 Jreen::MessageSession *session) :
	Jreen::MessageFilter(session),
	m_account(account)
{

}

void JMessageReceiptFilter::filter(Jreen::Message &message)
{
	Jreen::Receipt *receipt = message.payload<Jreen::Receipt>().data();
	ChatUnit *unit = m_account->conferenceManager()->muc(message.from());
	if (!unit)
		unit = m_account->roster()->contact(message.from(), true);
	if(message.containsPayload<Jreen::Error>())
		return;
	if(receipt) {
		if(receipt->type() == Jreen::Receipt::Received) {
			QString id = receipt->id();
			if(id.isEmpty())
				id = message.id(); //for slowpoke client such as Miranda			
			if(unit)
				qApp->postEvent(ChatLayer::get(unit),
								new qutim_sdk_0_3::MessageReceiptEvent(id.toUInt(), true));
		} else {
			//TODO send this request only when message marked as read
			Jreen::Message request(Jreen::Message::Chat,
								   message.from());
			request.setThread(message.thread());
			//for slowpoke clients
			request.setID(message.id());
			//correct behaviour
			request.addExtension(new Jreen::Receipt(Jreen::Receipt::Received,message.id()));
			m_account->client()->send(request);
		}
	}
	Jreen::ChatState *state = message.payload<Jreen::ChatState>().data();
	if(state) {
		if(unit)
			unit->setChatState(static_cast<qutim_sdk_0_3::ChatState>(state->state()));
	}
}

void JMessageReceiptFilter::decorate(Jreen::Message &message)
{
	Jreen::Receipt *receipt = new Jreen::Receipt(Jreen::Receipt::Request);
	message.addExtension(receipt);
}

int JMessageReceiptFilter::filterType() const
{
	return Jreen::Message::Chat;
}

void JMessageReceiptFilter::reset()
{

}

//dead code
//class JMessageHandlerPrivate
//{
//public:
//	JAccount *account;
//	QHash<QString, JMessageSession *> sessions;
//	QList<MessageFilterFactory*> filterFactories;
//};

//JMessageHandler::JMessageHandler(JAccount *account) : QObject(account), d_ptr(new JMessageHandlerPrivate)
//{
//	//Q_D(JMessageHandler);
//	//d->account = account;
//	//d->account->connection()->client()->registerMessageSessionHandler(this);
//	//foreach (const ObjectGenerator *ext, ObjectGenerator::module<MessageFilterFactory>())
//	//	d->filterFactories << ext->generate<MessageFilterFactory>();
//}

//JMessageHandler::~JMessageHandler()
//{
//}

//JAccount *JMessageHandler::account()
//{
//	return d_func()->account;
//}

//JMessageSession *JMessageHandler::getSession(const QString &id)
//{
//	return d_func()->sessions.value(id);
//}

//void JMessageHandler::handleMessageSession(MessageSession *session)
//{
//	//Q_D(JMessageHandler);
//	//// FIXME: Double conversion from JID to QString and from QString to JID
//	//ChatUnit *unit = d->account->getUnit(QString::fromStdString(session->target().full()), true);
//	//int types = ~0;
//	//if (qobject_cast<JMUCUser*>(unit))
//	//	types ^= gloox::Message::Groupchat;
//	//session->setTypes(types);
//	//if (qobject_cast<JMessageSessionOwner*>(unit)) {
//	//	Q_UNUSED(new JMessageSession(this, unit, session));
//	//} else {
//	//	debug() << "Cannot create JMessageSession for" << unit->id();
//	//}
//}

//void JMessageHandler::prepareMessageSession(JMessageSession *session)
//{
//	//Q_D(JMessageHandler);
//	//JabberParams params = d->account->connection()->params();
//	//foreach (MessageFilterFactory *factory, d->filterFactories) {
//	//	MessageFilter *filter = factory->create(d->account, params, session->session());
//	//	Q_UNUSED(filter);
//	//}
//}

//void JMessageHandler::setSessionId(JMessageSession *session, const QString &id)
//{
//	d_func()->sessions.insert(id, session);
//}

//void JMessageHandler::removeSessionId(const QString &id)
//{
//	d_func()->sessions.remove(id);
//}

//void JMessageHandler::createSession(ChatUnit *unit)
//{
//	//		Q_D(JMessageHandler);
//	//		if (qobject_cast<JMessageSessionOwner*>(unit)) {
//	//			int types = ~0;
//	//			if (qobject_cast<JMUCUser*>(unit))
//	//				types ^= gloox::Message::Groupchat;
//	////			MessageSession *glooxSession = new MessageSession(d->account->client(),
//	////															  unit->id().toStdString(),
//	////															  false, types, true);
//	//			Q_UNUSED(new JMessageSession(this, unit, glooxSession));
//	//		} else {
//	//			debug() << "Cannot create JMessageSession for" << unit->id();
//	//		}
//}

}

