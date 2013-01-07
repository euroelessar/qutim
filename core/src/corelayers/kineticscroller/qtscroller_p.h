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

#ifndef QTSCROLLER_P_H
#define QTSCROLLER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QSet>
#include <QEasingCurve>
#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
#  include <QTime>
typedef QTime QElapsedTimer;
#else
#  include <QElapsedTimer>
#endif
#include <QSizeF>
#include <QPointF>
#include <QRectF>
#include "qtscroller.h"
#include "qtscrollerproperties.h"
#include "qtscrollerproperties_p.h"
#include <QAbstractAnimation>

class QtFlickGestureRecognizer;

class QScrollTimer;

class QtScrollerPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(QtScroller)

public:
    QtScrollerPrivate(QtScroller *q, QObject *target);
    void init();

    void sendEvent(QObject *o, QEvent *e);

    void setState(QtScroller::State s);

    enum ScrollType {
        ScrollTypeFlick = 0,
        ScrollTypeScrollTo,
        ScrollTypeOvershoot
    };

    struct ScrollSegment {
        qint64 startTime;
        qint64 deltaTime;
        qreal startPos;
        qreal deltaPos;
        QEasingCurve curve;
        qreal stopProgress; // whatever is..
        qreal stopPos;      // ..reached first
        ScrollType type;
    };

    bool pressWhileInactive(const QPointF &position, qint64 timestamp);
    bool moveWhilePressed(const QPointF &position, qint64 timestamp);
    bool releaseWhilePressed(const QPointF &position, qint64 timestamp);
    bool moveWhileDragging(const QPointF &position, qint64 timestamp);
    bool releaseWhileDragging(const QPointF &position, qint64 timestamp);
    bool pressWhileScrolling(const QPointF &position, qint64 timestamp);

    void timerTick();
    void timerEventWhileDragging();
    void timerEventWhileScrolling();

    bool prepareScrolling(const QPointF &position);
    void handleDrag(const QPointF &position, qint64 timestamp);

    QPointF realDpi(int screen);
    QPointF dpi() const;
    void setDpi(const QPointF &dpi);
    void setDpiFromWidget(QWidget *widget);

    void updateVelocity(const QPointF &deltaPixelRaw, qint64 deltaTime);
    void pushSegment(ScrollType type, qreal deltaTime, qreal stopProgress, qreal startPos, qreal deltaPos, qreal stopPos, QEasingCurve::Type curve, Qt::Orientation orientation);
    void recalcScrollingSegments(bool force = false);
    qreal scrollingSegmentsEndPos(Qt::Orientation orientation) const;
    bool scrollingSegmentsValid(Qt::Orientation orientation);
    void createScrollToSegments(qreal v, qreal deltaTime, qreal endPos, Qt::Orientation orientation, ScrollType type);
    void createScrollingSegments(qreal v, qreal startPos, qreal ppm, Qt::Orientation orientation);

    void setContentPositionHelperDragging(const QPointF &deltaPos);
    void setContentPositionHelperScrolling();

    qreal nextSnapPos(qreal p, int dir, Qt::Orientation orientation);
    static qreal nextSegmentPosition(QQueue<ScrollSegment> &segments, qint64 now, qreal oldPos);

    inline int frameRateSkip() const { return properties.d.data()->frameRate; }

    static const char *stateName(QtScroller::State state);
    static const char *inputName(QtScroller::Input input);

public slots:
    void targetDestroyed();

public:
    // static
    static QMap<QObject *, QtScroller *> allScrollers;
    static QSet<QtScroller *> activeScrollers;

    // non static
    QObject *target;
    QtScrollerProperties properties;
    QtFlickGestureRecognizer *recognizer;
    Qt::GestureType recognizerType;

    // scroller state:

    // QPointer<QObject> scrollTarget;
    QSizeF viewportSize;
    QRectF contentPosRange;
    QPointF contentPosition;
    QPointF overshootPosition; // the number of pixels we are overshooting (before overshootDragResistanceFactor)

    // state

    bool enabled;
    QtScroller::State state;
    bool firstScroll; // true if we haven't already send a scroll event

    QPointF oldVelocity; // the release velocity of the last drag

    QPointF pressPosition;
    QPointF lastPosition;
    qint64  pressTimestamp;
    qint64  lastTimestamp;

    QPointF dragDistance; // the distance we should move during the next drag timer event

    QQueue<ScrollSegment> xSegments;
    QQueue<ScrollSegment> ySegments;

    // snap positions
    QList<qreal> snapPositionsX;
    qreal snapFirstX;
    qreal snapIntervalX;
    QList<qreal> snapPositionsY;
    qreal snapFirstY;
    qreal snapIntervalY;

    QPointF pixelPerMeter;

    QElapsedTimer monotonicTimer;

    QPointF releaseVelocity; // the starting velocity of the scrolling state
    QScrollTimer *scrollTimer;

    QtScroller *q_ptr;
};

#endif // QTSCROLLER_P_H

