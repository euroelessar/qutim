#include "jmessagehandler.h"
#include "jmessagesession.h"
#include "../jaccount.h"
#include "jcontact.h"
#include "jcontactresource.h"
#include "../muc/jmucuser.h"
#include "gloox/message.h"
#include "../../../sdk/jabber.h"
#include <qutim/debug.h>

namespace Jabber
{
	class JMessageHandlerPrivate
	{
	public:
		JAccount *account;
		QHash<QString, JMessageSession *> sessions;
		QList<MessageFilterFactory*> filterFactories;
	};

	JMessageHandler::JMessageHandler(JAccount *account) : QObject(account), d_ptr(new JMessageHandlerPrivate)
	{
		Q_D(JMessageHandler);
		d->account = account;
		d->account->connection()->client()->registerMessageSessionHandler(this);
		foreach (const ObjectGenerator *ext, moduleGenerators<MessageFilterFactory>())
			d->filterFactories << ext->generate<MessageFilterFactory>();
	}

	JMessageHandler::~JMessageHandler()
	{
	}

	JAccount *JMessageHandler::account()
	{
		return d_func()->account;
	}

	JMessageSession *JMessageHandler::getSession(const QString &id)
	{
		return d_func()->sessions.value(id);
	}

	void JMessageHandler::handleMessageSession(MessageSession *session)
	{
		Q_D(JMessageHandler);
		// FIXME: Double conversion from JID to QString and from QString to JID
		ChatUnit *unit = d->account->getUnit(QString::fromStdString(session->target().full()), true);
		int types = ~0;
		if (qobject_cast<JMUCUser*>(unit))
			types ^= gloox::Message::Groupchat;
		session->setTypes(types);
		if (qobject_cast<JMessageSessionOwner*>(unit)) {
			Q_UNUSED(new JMessageSession(this, unit, session));
		} else {
			debug() << "Cannot create JMessageSession for" << unit->id();
		}
	}
	
	void JMessageHandler::prepareMessageSession(JMessageSession *session)
	{
		Q_D(JMessageHandler);
		JabberParams params = d->account->connection()->params();
		foreach (MessageFilterFactory *factory, d->filterFactories) {
			MessageFilter *filter = factory->create(d->account, params, session->session());
			Q_UNUSED(filter);
		}
	}

	void JMessageHandler::setSessionId(JMessageSession *session, const QString &id)
	{
		d_func()->sessions.insert(id, session);
	}

	void JMessageHandler::removeSessionId(const QString &id)
	{
		d_func()->sessions.remove(id);
	}

	void JMessageHandler::createSession(ChatUnit *unit)
	{
		Q_D(JMessageHandler);
		if (qobject_cast<JMessageSessionOwner*>(unit)) {
			int types = ~0;
			if (qobject_cast<JMUCUser*>(unit))
				types ^= gloox::Message::Groupchat;
			MessageSession *glooxSession = new MessageSession(d->account->client(),
															  unit->id().toStdString(),
															  false, types, true);
			Q_UNUSED(new JMessageSession(this, unit, glooxSession));
		} else {
			debug() << "Cannot create JMessageSession for" << unit->id();
		}
	}

}
