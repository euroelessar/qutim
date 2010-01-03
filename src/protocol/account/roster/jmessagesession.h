#ifndef JMESSAGESESSION_H
#define JMESSAGESESSION_H

#include <qutim/chatunit.h>
#include <gloox/messagesession.h>
#include <gloox/messagehandler.h>
#include <gloox/chatstatehandler.h>

namespace Jabber
{
	struct JMessageSessionPrivate;
	class JMessageHandler;

	class JMessageSession
			: public qutim_sdk_0_3::ChatUnit,
			public gloox::MessageHandler,
			public gloox::ChatStateHandler
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JMessageSession)
	public:
		JMessageSession(JMessageHandler *handler, ChatUnit *unit, gloox::MessageSession *session);
		~JMessageSession();
		virtual QString id() const;
		virtual QString title() const;
		virtual void sendMessage(const qutim_sdk_0_3::Message &message);
		virtual void setChatState(qutim_sdk_0_3::ChatState state);
		virtual void handleMessage(const gloox::Message &msg, gloox::MessageSession* session = 0);
		virtual void handleChatState(const gloox::JID &from, gloox::ChatStateType state);
	private:
		QScopedPointer<JMessageSessionPrivate> d_ptr;
	};
}

#endif // JMESSAGESESSION_H
