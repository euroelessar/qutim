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
#ifndef QSCROLLEVENT_P_H
#define QSCROLLEVENT_P_H

#include "qtscrollevent.h"

class QtScrollPrepareEventPrivate
{
public:
    inline QtScrollPrepareEventPrivate()
        : target(0)
    {
    }

    QObject* target;
    QPointF startPos;
    QSizeF viewportSize;
    QRectF contentPosRange;
    QPointF contentPos;
};

class QtScrollEventPrivate
{
public:
    inline QtScrollEventPrivate()
    {
    }

    QPointF contentPos;
    QPointF overshoot;
    QtScrollEvent::ScrollState state;
};

#endif // QSCROLLEVENT_P_H

