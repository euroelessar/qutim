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
#include <memory>
#include <QThreadStorage>

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

static quint64 currentMessageIdHook = -1;

struct MessageHandler::StateType : public std::enable_shared_from_this<StateType>
{
    StateType(const Message &message, const MessageHandlerList &list, const MessageHandler::FinalHandler &handler)
        : index(0), message(message), messageId(message.id()), list(list), handler(handler)
    {
        this->message.setChatUnit(message.chatUnit());
    }

    void onResult(MessageHandler::Result result, const QString &error)
    {
        if (result != MessageHandler::Accept) {
            handler(message, result, error);
            return;
        }

        using namespace std::placeholders;
        if (index < list.size()) {
            currentMessageIdHook = messageId;
            list[index++].handler->doHandle(message, std::bind(&StateType::onResult, shared_from_this(), _1, _2));
        } else {
            handler(message, MessageHandler::Accept, QString());
        }
    }

    int index;
    Message message;
    quint64 messageId;
    const MessageHandlerList list;
    MessageHandler::FinalHandler handler;
};

void MessageHandler::handle(const Message &message, const FinalHandler &handler)
{
    const MessageHandlerList &list = (message.isIncoming() ? scope()->incoming : scope()->outgoing);

    if (list.isEmpty()) {
        handler(message, Accept, QString());
        return;
    }

    auto state = std::make_shared<StateType>(message, list, handler);
    state->onResult(Accept, QString());
}

void MessageHandler::traceHandlers()
{
	MessageHandlerListPtr lists[] = { &scope()->incoming, &scope()->outgoing };
	const char *titles[] = { "Incoming handlers:", "Outgoing handlers:" };
	for (int i = 0; i < 2; ++i) {
		MessageHandlerList &list = *lists[i];
		QDebug dbg = qDebug();
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

quint64 MessageHandler::originalMessageId()
{
    return currentMessageIdHook;
}

}

