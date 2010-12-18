#ifndef JMESSAGEHANDLER_H
#define JMESSAGEHANDLER_H

#include <QObject>
#include <gloox/messagesessionhandler.h>
#include <qutim/messagesession.h>
//jreen
#include <jreen/messagesession.h>

namespace qutim_sdk_0_3
{
class ChatUnit;
class Message;
}

namespace Jabber
{

class JAccount;
class JMessageSessionHandler : public jreen::MessageSessionHandler
{
public:
	JMessageSessionHandler(JAccount *account);
	virtual ~JMessageSessionHandler();
	virtual void handleMessageSession(jreen::MessageSession *session);
private:
	JAccount *m_account;
};

class JMessageSessionManagerPrivate;
class JMessageSessionManager : public jreen::MessageSessionManager
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JMessageSessionManager)
public:
	JMessageSessionManager(JAccount *account);
	virtual ~JMessageSessionManager();
	void sendMessage(const qutim_sdk_0_3::Message &message);
public slots:
	virtual void handleMessage(const jreen::Message &message);
private:
	QScopedPointer<JMessageSessionManagerPrivate> d_ptr;
};

class JMessageReceiptFilter : public jreen::MessageFilter
{
public:
	JMessageReceiptFilter(JAccount *account,jreen::MessageSession *session);
	virtual ~JMessageReceiptFilter() {}
	virtual void filter(jreen::Message &message);
	virtual void decorate(jreen::Message &message);
	virtual void reset();
	virtual int filterType() const;
private:
	JAccount *m_account;
};

//old code
//class JMessageSession;
//class JMessageHandlerPrivate;
//using qutim_sdk_0_3::ChatUnit;
//class JMessageHandler : public QObject, public gloox::MessageSessionHandler
//{
//	Q_OBJECT
//	Q_DECLARE_PRIVATE(JMessageHandler)
//public:
//	JMessageHandler(JAccount *account);
//	~JMessageHandler();
//	JAccount *account();
//	JMessageSession *getSession(const QString &id);
//	void handleMessageSession(gloox::MessageSession *session);
//	void prepareMessageSession(JMessageSession *session);
//	void setSessionId(JMessageSession *session, const QString &id);
//	void removeSessionId(const QString &id);
//	void createSession(ChatUnit *unit);
//private:
//	QScopedPointer<JMessageHandlerPrivate> d_ptr;
//};
}

#endif // JMESSAGEHANDLER_H
