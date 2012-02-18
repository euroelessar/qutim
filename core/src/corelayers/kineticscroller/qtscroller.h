/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nokia Corporation and/or its subsidiary(-ies)
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

#ifndef QTSCROLLER_H
#define QTSCROLLER_H

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include "qtscrollerproperties.h"

class QWidget;
class QtScrollerPrivate;
class QtScrollerProperties;
class QtFlickGestureRecognizer;

class QtScroller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QtScrollerProperties scrollerProperties READ scrollerProperties WRITE setScrollerProperties NOTIFY scrollerPropertiesChanged)
    Q_ENUMS(State)

public:
    enum State
    {
        Inactive,
        Pressed,
        Dragging,
        Scrolling
    };

    enum ScrollerGestureType
    {
        TouchGesture,
        LeftMouseButtonGesture,
        RightMouseButtonGesture,
        MiddleMouseButtonGesture
    };

    enum Input
    {
        InputPress = 1,
        InputMove,
        InputRelease
    };

    static bool hasScroller(QObject *target);

    static QtScroller *scroller(QObject *target);
    static const QtScroller *scroller(const QObject *target);

    static Qt::GestureType grabGesture(QObject *target, ScrollerGestureType gestureType = TouchGesture);
    static Qt::GestureType grabbedGesture(QObject *target);
    static void ungrabGesture(QObject *target);

    static QList<QtScroller *> activeScrollers();

    QObject *target() const;

    State state() const;

    bool handleInput(Input input, const QPointF &position, qint64 timestamp = 0);

    void stop();
    QPointF velocity() const;
    QPointF finalPosition() const;
    QPointF pixelPerMeter() const;

    QtScrollerProperties scrollerProperties() const;

    void setSnapPositionsX( const QList<qreal> &positions );
    void setSnapPositionsX( qreal first, qreal interval );
    void setSnapPositionsY( const QList<qreal> &positions );
    void setSnapPositionsY( qreal first, qreal interval );

public Q_SLOTS:
    void setScrollerProperties(const QtScrollerProperties &prop);
    void scrollTo(const QPointF &pos);
    void scrollTo(const QPointF &pos, int scrollTime);
    void ensureVisible(const QRectF &rect, qreal xmargin, qreal ymargin);
    void ensureVisible(const QRectF &rect, qreal xmargin, qreal ymargin, int scrollTime);
    void resendPrepareEvent();

Q_SIGNALS:
    void stateChanged(QtScroller::State newstate);
    void scrollerPropertiesChanged(const QtScrollerProperties &);

private:
    QtScrollerPrivate* d_ptr;

    QtScroller(QObject *target);
    virtual ~QtScroller();

    Q_DISABLE_COPY(QtScroller)
    Q_DECLARE_PRIVATE(QtScroller)

    friend class QtFlickGestureRecognizer;
};

#endif // QTSCROLLER_H

