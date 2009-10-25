/****************************************************************************
 *  messagesession.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
#include "objectgenerator.h"
#include "account.h"
#include <QPointer>

namespace qutim_sdk_0_3
{
	struct ChatSessionPrivate
	{
	};

	struct ChatLayerPrivate
	{
		QPointer<ChatLayer> self;
	};

	static ChatLayerPrivate *p = new ChatLayerPrivate;

	ChatSession::ChatSession(ChatLayer *chat) : QObject(chat), p(new ChatSessionPrivate)
	{
	}

	ChatSession::~ChatSession()
	{
	}

	ChatLayer::ChatLayer()
	{
	}

	ChatLayer::~ChatLayer()
	{
		if(p->self == this)
			p->self = NULL;
	}

	ChatLayer *ChatLayer::instance()
	{
		if(p->self.isNull() && isCoreInited())
		{
			GeneratorList list = moduleGenerators<ChatLayer>();
			if(!list.isEmpty())
				p->self = list.first()->generate<ChatLayer>();
		}
		return p->self;
	}

	ChatSession *ChatLayer::getSession(QObject *obj, bool create)
	{
		QVariant acc = obj->property("account");
		QVariant id = obj->property("id");
		if(!acc.isValid() || !id.isValid())
			return NULL;
		return getSession(acc.value<Account *>(), id.toString(), create);
	}
}
