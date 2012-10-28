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

namespace Ureen {

class PendingReplyData : public QSharedData
{
public:
    PendingReplyData() : resultReady(false) {}
    QList<PendingReplyBase::HandlerBase*> handlers;
    QVariantList result;
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
        handler->handle(data->result);
    }
    data->handlers << handler;
}

void PendingReplyBase::setResult_private(const QVariant &arg1, const QVariant &arg2,
                                         const QVariant &arg3, const QVariant &arg4,
                                         const QVariant &arg5, const QVariant &arg6,
                                         const QVariant &arg7, const QVariant &arg8)
{
    data->resultReady = true;
    data->result.clear();
    data->result << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8;
    while (!data->result.isEmpty() && data->result.last().isNull()) {
        data->result.removeLast();
    }
    foreach (HandlerBase *handler, data->handlers) {
        if (handler->object()) {
            handler->handle(data->result);
        }
    }
}

} // namespace Ureen
