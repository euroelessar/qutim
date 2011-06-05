/****************************************************************************
 *  messagesession.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "messagesession.h"
#include "messagehandler.h"
#include "objectgenerator.h"
#include "servicemanager.h"
#include "account.h"
#include <QPointer>
#include <QBasicTimer>
#include <QDateTime>
#include <numeric>

namespace qutim_sdk_0_3
{
typedef QMap<Message*, ChatSession*> MessageHookMap;
Q_GLOBAL_STATIC(MessageHookMap, messageHookMap)

class MessageHandlerHook : public MessageHandler
{
public:
	virtual Result doHandle(Message &message, QString *)
	{
		ChatSession *session = messageHookMap()->value(&message);
		if (session)
			session->doAppendMessage(message);
		return Accept;
	}
};

class ChatUnitSenderMessageHandler : public MessageHandler
{
public:
	virtual Result doHandle(Message &message, QString *)
	{
		if (!message.isIncoming()
		        && !message.property("service", false)
		        && !message.property("history", false)) {
			if (!message.chatUnit()->send(message))
				return Error;
		}
		return Accept;
	}
};

struct ChatSessionPrivate
{
};

struct ChatLayerData
{
	ServicePointer<ChatLayer> self;
	QScopedPointer<MessageHandlerHook> handlerHook;
	QScopedPointer<ChatUnitSenderMessageHandler> senderHook;
};

Q_GLOBAL_STATIC(ChatLayerData, p)

ChatSession::ChatSession(ChatLayer *chat) : QObject(chat), p(new ChatSessionPrivate)
{
}

ChatSession::~ChatSession()
{
}

qint64 ChatSession::append(qutim_sdk_0_3::Message &message)
{
	return appendMessage(message);
}

qint64 ChatSession::appendMessage(qutim_sdk_0_3::Message &message)
{
	QString reason;
	messageHookMap()->insert(&message, this);
	int result = MessageHandler::handle(message, &reason);
	if (MessageHandler::Accept != result) {
		// TODO optional user notification
//		Notifications:￼end(result)
		messageHookMap()->remove(&message);
		return -result;
	}
	messageHookMap()->remove(&message);
	return message.id();
}

void ChatSession::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

class ChatLayerPrivate
{
public:
	ChatLayerPrivate() : alerted(false) {}
	bool alerted;
	QBasicTimer alertTimer;
};

ChatLayer::ChatLayer() : d_ptr(new ChatLayerPrivate)
{
	p()->handlerHook.reset(new MessageHandlerHook);
	p()->senderHook.reset(new ChatUnitSenderMessageHandler);
	MessageHandler::registerHandler(p()->handlerHook.data(),
	                                MessageHandler::ChatInPriority,
	                                MessageHandler::ChatOutPriority);
	MessageHandler::registerHandler(p()->senderHook.data(),
	                                MessageHandler::NormalPriortity,
	                                MessageHandler::LowPriority);
}

ChatLayer::~ChatLayer()
{
	p()->handlerHook.reset(0);
}

ChatLayer *ChatLayer::instance()
{
	return p()->self.data();
}

ChatSession *ChatLayer::getSession(Account *acc, QObject *obj, bool create)
{
	QString id = (acc && obj) ? obj->property("id").toString() : QString();
	return id.isEmpty() ? 0 : getSession(acc->getUnit(id, create));
}

ChatSession *ChatLayer::getSession(QObject *obj, bool create)
{
	Account *acc = obj->property("account").value<Account *>();
	QString id = obj->property("id").toString();
	return getSession(acc, id, create);
}

ChatSession *ChatLayer::getSession(Account *acc, const QString &id, bool create)
{
	return (acc && !id.isEmpty()) ? getSession(acc->getUnit(id, create)) : 0;
}

ChatUnit *ChatLayer::getUnitForSession(ChatUnit *unit) const
{
	Account *acc = unit ? unit->account() : 0;
	return acc ? acc->getUnitForSession(unit) : unit;
}

ChatSession* ChatLayer::get(ChatUnit* unit, bool create)
{
	return instance() ? instance()->getSession(unit,create) : 0;
}

bool ChatLayer::isAlerted() const
{
	return d_func()->alerted;
}

void ChatLayer::alert(bool on)
{
	Q_D(ChatLayer);
	if (d->alerted == on)
		return;
	d->alerted = on;
	d->alertTimer.stop();
	emit alertStatusChanged(on);
}

void ChatLayer::alert(int msecs)
{
	Q_D(ChatLayer);
	d->alertTimer.start(msecs, this);
	if (!d->alerted) {
		d->alerted = true;
		emit alertStatusChanged(true);
	}
}

bool ChatLayer::event(QEvent *ev)
{
	if (ev->type() == QEvent::Timer) {
		QTimerEvent *timerEvent = static_cast<QTimerEvent*>(ev);
		if (timerEvent->timerId() == d_func()->alertTimer.timerId()) {
			d_func()->alertTimer.stop();
			alert(false);
			return true;
		}
	}
	return QObject::event(ev);
}

void ChatLayer::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}
}
