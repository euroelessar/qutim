/****************************************************************************
 *  history.cpp
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

#include "history.h"
#include "objectgenerator.h"
#include "buddy.h"
#include "metacontact.h"

namespace qutim_sdk_0_3
{
	struct Private
	{
		QPointer<History> self;
	};
	static Private *p = NULL;

	void ensurePrivate_helper()
	{
		p = new Private;
		GeneratorList gens = moduleGenerators<History>();
		if(!gens.isEmpty())
		   p->self = gens.first()->generate<History>();
	}
	inline void ensurePrivate()
	{ if(!p) ensurePrivate_helper(); }

	History::History()
	{
		ensurePrivate();
	}

	History::~History()
	{
	}

	History *History::instance()
	{
		ensurePrivate();
		if(p->self.isNull() && isCoreInited())
			p->self = new History();
		return p->self;
	}

	void History::store(const Message &message)
	{
		if(p->self.isNull() || p->self == this)
			return;
		p->self->store(message);
	}

	MessageList History::read(const ChatUnit *unit, const QDateTime &from, const QDateTime &to, int max_num)
	{
		if(p->self.isNull() || p->self == this)
			return MessageList();
		return p->self->read(unit, from, to, max_num);
	}

	void History::showHistory(const ChatUnit *unit)
	{
		if(p->self.isNull() || p->self == this)
			return;
		p->self->showHistory(unit);
	}

	void History::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}

	const ChatUnit *History::historyUnit(const ChatUnit *unit) const
	{
		ChatUnit *u = const_cast<ChatUnit*>(unit);
		ChatUnit *buddy = 0;
		if (qobject_cast<Contact*>(u))
			return u;
		else if (qobject_cast<Buddy*>(u))
			buddy = u;
		ChatUnit *p;
		while ((p = u->upperUnit()) != 0) {
			if (qobject_cast<MetaContact*>(p)) {
				break;
			} else if (qobject_cast<Contact*>(p)) {
				buddy = p;
				break;
			} else if (qobject_cast<Buddy*>(p) && !buddy) {
				buddy = p;
			}
			u = p;
		}
		return buddy ? buddy : unit;
	}
}
