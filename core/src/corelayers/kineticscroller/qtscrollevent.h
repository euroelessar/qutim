/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QTSCROLLEVENT_H
#define QTSCROLLEVENT_H

#include <QEvent>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QScopedPointer>


class QtScrollPrepareEventPrivate;
class QtScrollPrepareEvent : public QEvent
{
public:
    enum { ScrollPrepare = 15056 }; // random number

    QtScrollPrepareEvent(const QPointF &startPos);
    ~QtScrollPrepareEvent();

    QPointF startPos() const;

    QSizeF viewportSize() const;
    QRectF contentPosRange() const;
    QPointF contentPos() const;

    void setViewportSize(const QSizeF &size);
    void setContentPosRange(const QRectF &rect);
    void setContentPos(const QPointF &pos);

private:
    QScopedPointer<QtScrollPrepareEventPrivate> d;
};


class QtScrollEventPrivate;
class QtScrollEvent : public QEvent
{
public:
    enum { Scroll = 15057 }; // random number

    enum ScrollState
    {
        ScrollStarted,
        ScrollUpdated,
        ScrollFinished
    };

    QtScrollEvent(const QPointF &contentPos, const QPointF &overshoot, ScrollState scrollState);
    ~QtScrollEvent();

    QPointF contentPos() const;
    QPointF overshootDistance() const;
    ScrollState scrollState() const;

private:
    QScopedPointer<QtScrollEventPrivate> d;
};

#endif // QTSCROLLEVENT_H

