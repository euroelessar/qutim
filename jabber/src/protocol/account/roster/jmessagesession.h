#ifndef JMESSAGESESSION_H
#define JMESSAGESESSION_H

#include <qutim/buddy.h>
#include <gloox/messagesession.h>
#include <gloox/messagehandler.h>
#include <gloox/chatstatehandler.h>

//namespace qutim_sdk_0_3
//{
//class ChatUnit;
//}

//namespace Jabber
//{
//using namespace qutim_sdk_0_3;

//class JMessageSessionPrivate;
//class JMessageHandler;
//class JSessionConvertor;

//class JMessageSession
//		: public QObject,
//		public gloox::MessageHandler,
//		public gloox::ChatStateHandler
//{
//	Q_OBJECT
//	Q_DECLARE_PRIVATE(JMessageSession)
//public:
//	JMessageSession(JMessageHandler *handler, qutim_sdk_0_3::ChatUnit *unit, gloox::MessageSession *session);
//	JMessageSession(qutim_sdk_0_3::ChatUnit *unit);
//	~JMessageSession();
//	void convertToMuc();
//	QString id() const;
//	gloox::MessageSession *session();
//	bool sendMessage(const qutim_sdk_0_3::Message &message);
//	virtual void handleMessage(const gloox::Message &msg, gloox::MessageSession* session = 0);
//	virtual void handleChatState(const gloox::JID &from, gloox::ChatStateType state);
//	ChatUnit *upperUnit();
//	bool event(QEvent *);
//private:
//	QScopedPointer<JMessageSessionPrivate> d_ptr;
//	friend class JSessionConvertor;
//};
//}

#endif // JMESSAGESESSION_H
