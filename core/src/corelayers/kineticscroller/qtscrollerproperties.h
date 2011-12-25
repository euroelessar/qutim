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

#ifndef QTSCROLLERPROPERTIES_H
#define QTSCROLLERPROPERTIES_H

#include <QtCore/QScopedPointer>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>

#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
#  include <QEasingCurve>
Q_DECLARE_METATYPE(QEasingCurve)
#endif

class QtScroller;
class QtScrollerPrivate;
class QtScrollerPropertiesPrivate;

class QtScrollerProperties
{
public:
    QtScrollerProperties();
    QtScrollerProperties(const QtScrollerProperties &sp);
    QtScrollerProperties &operator=(const QtScrollerProperties &sp);
    virtual ~QtScrollerProperties();

    bool operator==(const QtScrollerProperties &sp) const;
    bool operator!=(const QtScrollerProperties &sp) const;

    static void setDefaultScrollerProperties(const QtScrollerProperties &sp);
    static void unsetDefaultScrollerProperties();

    enum OvershootPolicy
    {
        OvershootWhenScrollable,
        OvershootAlwaysOff,
        OvershootAlwaysOn
    };

    enum FrameRates {
        Standard,
        Fps60,
        Fps30,
        Fps20
    };

    enum ScrollMetric
    {
        MousePressEventDelay,                    // qreal [s]
        DragStartDistance,                       // qreal [m]
        DragVelocitySmoothingFactor,             // qreal [0..1/s]  (complex calculation involving time) v = v_new* DASF + v_old * (1-DASF)
        AxisLockThreshold,                       // qreal [0..1] atan(|min(dx,dy)|/|max(dx,dy)|)

        ScrollingCurve,                          // QEasingCurve
        DecelerationFactor,                      // slope of the curve

        MinimumVelocity,                         // qreal [m/s]
        MaximumVelocity,                         // qreal [m/s]
        MaximumClickThroughVelocity,             // qreal [m/s]

        AcceleratingFlickMaximumTime,            // qreal [s]
        AcceleratingFlickSpeedupFactor,          // qreal [1..]

        SnapPositionRatio,                       // qreal [0..1]
        SnapTime,                                // qreal [s]

        OvershootDragResistanceFactor,           // qreal [0..1]
        OvershootDragDistanceFactor,             // qreal [0..1]
        OvershootScrollDistanceFactor,           // qreal [0..1]
        OvershootScrollTime,                     // qreal [s]

        HorizontalOvershootPolicy,               // enum OvershootPolicy
        VerticalOvershootPolicy,                 // enum OvershootPolicy
        FrameRate,                               // enum FrameRates

        ScrollMetricCount
    };

    QVariant scrollMetric(ScrollMetric metric) const;
    void setScrollMetric(ScrollMetric metric, const QVariant &value);

protected:
    QScopedPointer<QtScrollerPropertiesPrivate> d;

private:
    QtScrollerProperties(QtScrollerPropertiesPrivate &dd);

    friend class QtScrollerPropertiesPrivate;
    friend class QtScroller;
    friend class QtScrollerPrivate;
};

Q_DECLARE_METATYPE(QtScrollerProperties::OvershootPolicy)
Q_DECLARE_METATYPE(QtScrollerProperties::FrameRates)

#endif // QTSCROLLERPROPERTIES_H

