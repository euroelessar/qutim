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
#ifndef JMESSAGEHANDLER_H
#define JMESSAGEHANDLER_H

#include <QObject>
//#include <gloox/messagesessionhandler.h>
#include <qutim/chatsession.h>
//Jreen
#include <jreen/messagesession.h>

namespace qutim_sdk_0_3
{
class ChatUnit;
class Message;
}

namespace Jabber
{

class JAccount;
class JMessageSessionHandler : public Jreen::MessageSessionHandler
{
public:
	JMessageSessionHandler(JAccount *account);
	virtual ~JMessageSessionHandler();
	virtual void handleMessageSession(Jreen::MessageSession *session);
private:
	JAccount *m_account;
};

class JMessageSessionManagerPrivate;
class JMessageSessionManager : public Jreen::MessageSessionManager
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JMessageSessionManager)
public:
	JMessageSessionManager(JAccount *account);
	virtual ~JMessageSessionManager();
	void sendMessage(qutim_sdk_0_3::ChatUnit *unit, const qutim_sdk_0_3::Message &message);
public slots:
	virtual void handleMessage(const Jreen::Message &message);
private:
	QScopedPointer<JMessageSessionManagerPrivate> d_ptr;
};

class JMessageReceiptFilter : public Jreen::MessageFilter
{
public:
	JMessageReceiptFilter(JAccount *account,Jreen::MessageSession *session);
	virtual ~JMessageReceiptFilter() {}
	virtual void filter(Jreen::Message &message);
	virtual void decorate(Jreen::Message &message);
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

