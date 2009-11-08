/****************************************************************************
 *  history.cpp
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

#include "history.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	struct Private
	{
		QPointer<History> self;
	};
	static Private *p = NULL;

	History::History()
	{
		if(!p)
		{
			p = new Private;
			GeneratorList gens = moduleGenerators<History>();
			if(!gens.isEmpty())
			   p->self = gens.first()->generate<History>();
		}
	}

	History::~History()
	{
	}

	History *History::instance()
	{
		static QPointer<History> self;
		if(self.isNull())
			self = new History();
		return self;
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

	void History::showHistory()
	{
		if(p->self.isNull() || p->self == this)
			return;
		p->self->showHistory();
	}
}
