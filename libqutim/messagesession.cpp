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
}
