#ifndef JMESSAGEHANDLER_H
#define JMESSAGEHANDLER_H

#include <QObject>
#include <gloox/messagesessionhandler.h>
#include <qutim/messagesession.h>

namespace qutim_sdk_0_3
{
	class ChatUnit;
}

namespace Jabber
{
	using qutim_sdk_0_3::ChatUnit;
	class JAccount;
	class JMessageSession;
	class JMessageHandlerPrivate;

	class JMessageHandler : public QObject, public gloox::MessageSessionHandler
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JMessageHandler)
	public:
		JMessageHandler(JAccount *account);
		~JMessageHandler();
		JAccount *account();
		JMessageSession *getSession(const QString &id);
		void handleMessageSession(gloox::MessageSession *session);
		void prepareMessageSession(JMessageSession *session);
		void setSessionId(JMessageSession *session, const QString &id);
		void removeSessionId(const QString &id);
		void createSession(ChatUnit *unit);
	private:
		QScopedPointer<JMessageHandlerPrivate> d_ptr;
	};
}

#endif // JMESSAGEHANDLER_H
