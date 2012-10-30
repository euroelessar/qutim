/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "pendingreply.h"
#include <QSharedData>
#include <cstring>

namespace Ureen {

class PendingReplyData : public QSharedData
{
public:
    PendingReplyData() : resultReady(false) {}
    QList<PendingReplyBase::HandlerBase*> handlers;
    Internal::Variable::PtrList result;
    bool resultReady;
};

PendingReplyBase::PendingReplyBase(int) : data(new PendingReplyData)
{
}

PendingReplyBase::PendingReplyBase(const PendingReplyBase &rhs) : data(rhs.data)
{
}

PendingReplyBase &PendingReplyBase::operator=(const PendingReplyBase &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

PendingReplyBase::~PendingReplyBase()
{
}

void PendingReplyBase::addHandler(PendingReplyBase::HandlerBase *handler)
{
    if (data->resultReady) {
        void *argsData[9];
        memset(argsData, sizeof(argsData), 0);
        for (int i = 0; i < data->result.size(); ++i)
            argsData[i] = data->result[i]->data();
        handler->handle(data->result.size(), argsData);
    }
    data->handlers << handler;
}

void PendingReplyBase::setResult_private(const Internal::Variable::PtrList &args)
{
    data->result = args;
    if (data->handlers.isEmpty()) {
        void *argsData[9];
        memset(argsData, sizeof(argsData), 0);
        for (int i = 0; i < args.size(); ++i)
            argsData[i] = args[i]->data();
        foreach (HandlerBase *handler, data->handlers) {
            if (handler->object()) {
                handler->handle(args.size(), argsData);
            }
        }
    }
}

} // namespace Ureen
