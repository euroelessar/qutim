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

struct ChatSessionPrivate
{
};

struct ChatLayerPrivate
{
	QPointer<ChatLayer> self;
	QScopedPointer<MessageHandlerHook> handlerHook;
};

static ChatLayerPrivate *p = new ChatLayerPrivate;

ChatSession::ChatSession(ChatLayer *chat) : QObject(chat), p(new ChatSessionPrivate)
{
}

ChatSession::~ChatSession()
{
}

qint64 ChatSession::appendMessage(qutim_sdk_0_3::Message &message)
{
	QString reason;
	messageHookMap()->insert(&message, this);
	if (MessageHandler::Accept != MessageHandler::handle(message, &reason)) {
		// TODO optional user notification
//		Notifications:￼end(result)
		messageHookMap()->remove(&message);
		return -1;
	}
	messageHookMap()->remove(&message);
	return message.id();
}

void ChatSession::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

ChatLayer::ChatLayer()
{
	p->handlerHook.reset(new MessageHandlerHook);
	MessageHandler::registerHandler(p->handlerHook.data(),
	                                MessageHandler::ChatInPriority,
	                                MessageHandler::ChatOutPriority);
}

ChatLayer::~ChatLayer()
{
	p->handlerHook.reset(0);
}

ChatLayer *ChatLayer::instance()
{
	if(p->self.isNull() && ObjectGenerator::isInited()) {
		p->self = qobject_cast<ChatLayer*>(ServiceManager::getByName("ChatLayer"));
		Q_ASSERT(p->self);
	}
	return p->self;
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

void ChatLayer::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}
}
