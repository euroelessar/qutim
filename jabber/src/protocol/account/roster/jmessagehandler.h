#ifndef JMESSAGEHANDLER_H
#define JMESSAGEHANDLER_H

#include <QObject>
#include <gloox/messagesessionhandler.h>
#include <qutim/messagesession.h>

namespace Jabber
{
	class JAccount;
	class JMessageSession;
	struct JMessageHandlerPrivate;

	class JMessageHandler : public QObject, public gloox::MessageSessionHandler
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JMessageHandler)
	public:
		JMessageHandler(JAccount *account);
		~JMessageHandler();

		JAccount *account();

		qutim_sdk_0_3::ChatUnit *getSession(qutim_sdk_0_3::ChatUnit *unit, bool create = true);
		qutim_sdk_0_3::ChatUnit *getSession(const QString &id);
		void handleMessageSession(gloox::MessageSession *session);
		void prepareMessageSession(JMessageSession *session);
		void messageSessionKiled(JMessageSession *session);
		void setSessionId(JMessageSession *session, const QString &id);
		void setSessionUnit(JMessageSession *session, qutim_sdk_0_3::ChatUnit *unit);
	private:
		QScopedPointer<JMessageHandlerPrivate> d_ptr;
	};
}

#endif // JMESSAGEHANDLER_H
