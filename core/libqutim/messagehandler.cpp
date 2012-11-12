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

#include "messagehandler.h"
#include "debug.h"

namespace qutim_sdk_0_3
{

struct MessageHandlerInfo
{
	int priority;
	QString name;
	MessageHandler *handler;
	
	bool operator <(const MessageHandlerInfo &o) const
	{
		return priority < o.priority || (priority == o.priority && handler < o.handler);
	}
};

typedef QList<MessageHandlerInfo> MessageHandlerList;
typedef MessageHandlerList* MessageHandlerListPtr;
struct Scope
{
	MessageHandlerList incoming;
	MessageHandlerList outgoing;
};

Q_GLOBAL_STATIC(Scope, scope)

MessageHandler::~MessageHandler()
{
	if (scope())
		unregisterHandler(this);
}

void MessageHandler::registerHandler(MessageHandler *handler, int incomingPriority, int outgoingPriority)
{
	registerHandler(handler, QString(), incomingPriority, outgoingPriority);
}

void MessageHandler::registerHandler(MessageHandler *handler, const QString &name, int incomingPriority, int outgoingPriority)
{
	MessageHandlerListPtr lists[] = { &scope()->incoming, &scope()->outgoing };
	int priorities[] = { incomingPriority, outgoingPriority };
	for (int i = 0; i < 2; ++i) {
		MessageHandlerInfo info = { priorities[i], name, handler };
		int index = qUpperBound(lists[i]->constBegin(),
								lists[i]->constEnd(),
								info,
								qGreater<MessageHandlerInfo>())
					- lists[i]->constBegin();
		lists[i]->insert(index, info);
	}
}

void MessageHandler::unregisterHandler(MessageHandler *handler)
{
	MessageHandlerListPtr lists[] = { &scope()->incoming, &scope()->outgoing };
	bool found = false;
	for (int i = 0; i < 1 || (i == 1 && found); ++i) {
		for (int j = 0; j < lists[i]->size(); ++j) {
			if (lists[i]->at(j).handler == handler) {
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
		Result result = list.at(i).handler->doHandle(message, reason);
		if (result != Accept)
			return result;
	}
	return Accept;
}

void MessageHandler::traceHandlers()
{
	MessageHandlerListPtr lists[] = { &scope()->incoming, &scope()->outgoing };
	const char *titles[] = { "Incoming handlers:", "Outgoing handlers:" };
	for (int i = 0; i < 2; ++i) {
		MessageHandlerList &list = *lists[i];
		QDebug dbg = debug();
		dbg << titles[i];
		dbg.nospace();
		for (int j = 0; j < list.size(); ++j) {
			MessageHandlerInfo &info = list[j];
			if (j > 0)
				dbg << " -> ";
			dbg << "(0x" << QByteArray::number(info.priority, 16).constData() << ", " << info.name << ")";
		}
	}
}

}

