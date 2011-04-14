/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "messagehandler.h"

namespace qutim_sdk_0_3
{

typedef QList<QPair<int, MessageHandler*> > MessageHandlerList;
typedef MessageHandlerList* MessageHandlerListPtr;
struct Scope
{
	MessageHandlerList incoming;
	MessageHandlerList outgoing;
};

Q_GLOBAL_STATIC(Scope, scope)

MessageHandler::~MessageHandler()
{
	unregisterHandler(this);
}

void MessageHandler::registerHandler(MessageHandler *handler, int incomingPriority, int outgoingPriority)
{
	MessageHandlerListPtr lists[] = { &scope()->incoming, &scope()->outgoing };
	int priorities[] = { incomingPriority, outgoingPriority };
	for (int i = 0; i < 2; ++i) {
		int index = qUpperBound(lists[i]->constBegin(),
		                        lists[i]->constEnd(),
		                        qMakePair(priorities[i], handler),
		                        qGreater<QPair<int, MessageHandler*> >())
		        - lists[i]->constBegin();
		lists[i]->insert(index, qMakePair(priorities[i], handler));
	}
}

void MessageHandler::unregisterHandler(MessageHandler *handler)
{
	MessageHandlerListPtr lists[] = { &scope()->incoming, &scope()->outgoing };
	bool found = false;
	for (int i = 0; i < 1 || (i == 1 && found); ++i) {
		for (int j = 0; j < lists[i]->size(); ++j) {
			if (lists[i]->at(j).second == handler) {
				lists[i]->removeAt(j);
				found = true;
			}
		}
	}
}

MessageHandler::Result MessageHandler::handle(Message &message, QString *reason)
{
	MessageHandlerList &list = (message.isIncoming() ? scope()->incoming : scope()->outgoing);
	QString tmp;
	if (!reason)
		reason = &tmp;
	for (int i = 0; i < list.size(); ++i) {
		reason->clear();
		Result result = list.at(i).second->doHandle(message, reason);
		if (result != Accept)
			return result;
	}
	return Accept;
}

}
