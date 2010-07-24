#include "jmessagehandler.h"
#include "jmessagesession.h"
#include "../jaccount.h"
#include "jcontact.h"
#include "jcontactresource.h"
#include "../muc/jmucuser.h"
#include "gloox/message.h"
#include "../../../sdk/jabber.h"

namespace Jabber
{
	struct JMessageHandlerPrivate
	{
		JAccount *account;
		QHash<QString, JMessageSession *> sessions;
		QHash<ChatUnit *, JMessageSession *> unitSessions;
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

	void JMessageHandler::handleMessageSession(MessageSession *session)
	{
		Q_D(JMessageHandler);
		// FIXME: Double conversion from JID to QString and from QString to JID
		ChatUnit *unit = d->account->getUnit(QString::fromStdString(session->target().full()), true);
		d->unitSessions.insert(unit, new JMessageSession(this, unit, session));
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
	
	void JMessageHandler::messageSessionKiled(JMessageSession *session)
	{
		Q_D(JMessageHandler);
		d->unitSessions.remove(d->unitSessions.key(session));
	}

	ChatUnit *JMessageHandler::getSession(ChatUnit *unit, bool create)
	{
		Q_D(JMessageHandler);
		JMessageSession *session = 0;
		if (!!(session = qobject_cast<JMessageSession *>(unit)))
			return session;
		if (!!(session = d->unitSessions.value(unit)))
			return session;
		if (qobject_cast<JContact *>(unit) || qobject_cast<JContactResource *>(unit)) {
			if (!create)
				return 0;
			int types = ~0;
			if (qobject_cast<JMUCUser*>(unit))
				types ^= gloox::Message::Groupchat;
			MessageSession *glooxSession = new MessageSession(d->account->client(),
															  unit->id().toStdString(),
															  false, types, true);
			d->unitSessions.insert(unit, session = new JMessageSession(this, unit, glooxSession));
			return session;
		}
		return unit;
	}

	ChatUnit *JMessageHandler::getSession(const QString &id)
	{
		return d_func()->sessions.value(id);
	}

	void JMessageHandler::setSessionId(JMessageSession *session, const QString &id)
	{
		d_func()->sessions.insert(id, session);
	}

	void JMessageHandler::setSessionUnit(JMessageSession *session, qutim_sdk_0_3::ChatUnit *unit)
	{
		d_func()->unitSessions.insert(unit, session);
	}
}
