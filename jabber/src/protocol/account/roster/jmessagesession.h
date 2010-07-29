#ifndef JMESSAGESESSION_H
#define JMESSAGESESSION_H

#include <qutim/buddy.h>
#include <gloox/messagesession.h>
#include <gloox/messagehandler.h>
#include <gloox/chatstatehandler.h>

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	struct JMessageSessionPrivate;
	class JMessageHandler;
	class JSessionConvertor;

	class JMessageSession
			: public qutim_sdk_0_3::Buddy,
			public gloox::MessageHandler,
			public gloox::ChatStateHandler
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JMessageSession)
	public:
		JMessageSession(JMessageHandler *handler, ChatUnit *unit, gloox::MessageSession *session);
		JMessageSession(ChatUnit *unit);
		~JMessageSession();
		void convertToMuc();
		virtual QString id() const;
		virtual QString title() const;
		gloox::MessageSession *session();
		virtual bool sendMessage(const qutim_sdk_0_3::Message &message);
		virtual void handleMessage(const gloox::Message &msg, gloox::MessageSession* session = 0);
		virtual void handleChatState(const gloox::JID &from, gloox::ChatStateType state);
		ChatUnit *upperUnit();
		ChatUnitList lowerUnits();
		bool event(QEvent *);
	private slots:
		void onUnitDeath();
		void onLowerUnitAdded(ChatUnit *unit);
	private:
		QScopedPointer<JMessageSessionPrivate> d_ptr;
		friend class JSessionConvertor;
	};
}

#endif // JMESSAGESESSION_H
