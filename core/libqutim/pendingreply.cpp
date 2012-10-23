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
    QList<PendingReplyBase::HandlerBase*> handlers;
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
    data->handlers << handler;
}

} // namespace Ureen
