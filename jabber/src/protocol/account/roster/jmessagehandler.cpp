#include "jmessagehandler.h"
#include "jmessagesession.h"
#include "../jaccount.h"
#include "jcontact.h"
#include "jcontactresource.h"
#include "../muc/jmucuser.h"
#include "../../../sdk/jabber.h"
#include <qutim/debug.h>
#include <jreen/delayeddelivery.h>
#include <jreen/receipt.h>
#include <jreen/client.h>
#include "jroster.h"

namespace Jabber
{
//jreen

JMessageSessionHandler::JMessageSessionHandler(JAccount *account) :
	m_account(account)
{

}

JMessageSessionHandler::~JMessageSessionHandler()
{
}

void JMessageSessionHandler::handleMessageSession(jreen::MessageSession *session)
{
	qDebug() << session;
	Q_ASSERT(session);
	session->registerMessageFilter(new JMessageReceiptFilter(m_account,session));
}

class JMessageSessionManagerPrivate
{
public:
	JMessageSessionManagerPrivate(JMessageSessionManager *q) : q_ptr(q) {}
	JMessageSessionManager *q_ptr;
	JAccount *account;
};

JMessageSessionManager::JMessageSessionManager(JAccount *account) :
	jreen::MessageSessionManager(account->client()),
	d_ptr(new JMessageSessionManagerPrivate(this))
{
	Q_D(JMessageSessionManager);
	d->account = account;
	QList<jreen::Message::Type> types;
	types.append(jreen::Message::Chat);
	types.append(jreen::Message::Headline);

	registerMessageSessionHandler(new JMessageSessionHandler(account),types);
}

JMessageSessionManager::~JMessageSessionManager()
{

}

void JMessageSessionManager::handleMessage(const jreen::Message &message)
{
	debug() << "handle message";
	return jreen::MessageSessionManager::handleMessage(message);
}

void JMessageSessionManager::sendMessage(const qutim_sdk_0_3::Message &message)
{
	jreen::JID jid(message.chatUnit()->id());
	jreen::MessageSession *s = session(jid,
									   jreen::Message::Chat,
									   true);

	jreen::Message msg(jreen::Message::Chat,
					   jid,
					   message.text(),
					   message.property("subject").toString());
	msg.setID(QString::number(message.id()));
	s->sendMessage(msg);
}

JMessageReceiptFilter::JMessageReceiptFilter(JAccount *account,
											 jreen::MessageSession *session) :
	jreen::MessageFilter(session),
	m_account(account)
{

}

void JMessageReceiptFilter::filter(jreen::Message &message)
{
	jreen::Receipt *receipt = message.findExtension<jreen::Receipt>().data();
	if(receipt) {
		if(receipt->type() == jreen::Receipt::Received) {
			QString id = receipt->id();
			if(id.isEmpty())
				id = message.id(); //for slowpoke client such as Miranda
			ChatUnit *unit = m_account->roster()->contact(message.from(),false);
			if(!unit)
				return;
			qApp->postEvent(ChatLayer::get(unit),
							new qutim_sdk_0_3::MessageReceiptEvent(id.toUInt(), true));
		} else {
			//only for testing
			//TODO send this request only when message marked as read
			jreen::Message request(jreen::Message::Chat,
								   message.from());
			//for slowpoke clients
			request.setID(message.id());
			//correct behaviour
			request.addExtension(new jreen::Receipt(jreen::Receipt::Received,message.id()));
			m_account->client()->send(request);
		}
	}
}

void JMessageReceiptFilter::decorate(jreen::Message &message)
{
	jreen::Receipt *receipt = new jreen::Receipt(jreen::Receipt::Request);
	message.addExtension(receipt);
}

int JMessageReceiptFilter::filterType() const
{
	return jreen::Message::Chat;
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
