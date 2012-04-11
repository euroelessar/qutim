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

#include <QWeakPointer>
#include <QObject>
#include <QtCore/qmath.h>

#ifdef Q_WS_WIN
#  include <QLibrary>
#endif

#include "qtscrollerproperties.h"
#include "qtscrollerproperties_p.h"

static QtScrollerPropertiesPrivate *userDefaults = 0;
static QtScrollerPropertiesPrivate *systemDefaults = 0;

QtScrollerPropertiesPrivate *QtScrollerPropertiesPrivate::defaults()
{
    if (!systemDefaults) {
        QtScrollerPropertiesPrivate spp;
#ifdef Q_WS_MAEMO_5
        spp.mousePressEventDelay = qreal(0);
        spp.dragStartDistance = qreal(2.5 / 1000);
        spp.dragVelocitySmoothingFactor = qreal(0.85);
        spp.axisLockThreshold = qreal(0);
        spp.scrollingCurve.setType(QEasingCurve::OutQuad);
        spp.decelerationFactor = 1.0;
        spp.minimumVelocity = qreal(0.0195);
        spp.maximumVelocity = qreal(6.84);
        spp.maximumClickThroughVelocity = qreal(0.0684);
        spp.acceleratingFlickMaximumTime = qreal(0.125);
        spp.acceleratingFlickSpeedupFactor = qreal(3.0);
        spp.snapPositionRatio = qreal(0.25);
        spp.snapTime = qreal(1);
        spp.overshootDragResistanceFactor = qreal(1);
        spp.overshootDragDistanceFactor = qreal(0.3);
        spp.overshootScrollDistanceFactor = qreal(0.3);
        spp.overshootScrollTime = qreal(0.5);
        spp.hOvershootPolicy = QtScrollerProperties::OvershootWhenScrollable;
        spp.vOvershootPolicy = QtScrollerProperties::OvershootWhenScrollable;
        spp.frameRate = QtScrollerProperties::Fps30;
#else
        spp.mousePressEventDelay = qreal(0.25);
        spp.dragStartDistance = qreal(5.0 / 1000);
        spp.dragVelocitySmoothingFactor = qreal(0.8);
        spp.axisLockThreshold = qreal(0);
        spp.scrollingCurve.setType(QEasingCurve::OutQuad);
        spp.decelerationFactor = qreal(0.125);
        spp.minimumVelocity = qreal(50.0 / 1000);
        spp.maximumVelocity = qreal(500.0 / 1000);
        spp.maximumClickThroughVelocity = qreal(66.5 / 1000);
        spp.acceleratingFlickMaximumTime = qreal(1.25);
        spp.acceleratingFlickSpeedupFactor = qreal(3.0);
        spp.snapPositionRatio = qreal(0.5);
        spp.snapTime = qreal(0.3);
        spp.overshootDragResistanceFactor = qreal(0.5);
        spp.overshootDragDistanceFactor = qreal(1);
        spp.overshootScrollDistanceFactor = qreal(0.5);
        spp.overshootScrollTime = qreal(0.7);
#  ifdef Q_WS_WIN
        if (QLibrary::resolve(QLatin1String("UxTheme"), "BeginPanningFeedback"))
            spp.overshootScrollTime = qreal(0.35);
#  endif
        spp.hOvershootPolicy = QtScrollerProperties::OvershootWhenScrollable;
        spp.vOvershootPolicy = QtScrollerProperties::OvershootWhenScrollable;
        spp.frameRate = QtScrollerProperties::Standard;
#endif
        systemDefaults = new QtScrollerPropertiesPrivate(spp);
    }
    return new QtScrollerPropertiesPrivate(userDefaults ? *userDefaults : *systemDefaults);
}

/*!
    \class QtScrollerProperties
    \brief The QtScrollerProperties class stores the settings for a QtScroller.
    \since 4.8

    The QtScrollerProperties class stores the parameters used by QtScroller.

    The default settings are platform dependent so that Qt emulates the
    platform behaviour for kinetic scrolling.

    As a convention the QtScrollerProperties are in physical units (meter,
    seconds) and are converted by QtScroller using the current DPI.

    \sa QtScroller
*/

/*!
    Constructs new scroller properties.
*/
QtScrollerProperties::QtScrollerProperties()
    : d(QtScrollerPropertiesPrivate::defaults())
{
}

/*!
    Constructs a copy of \a sp.
*/
QtScrollerProperties::QtScrollerProperties(const QtScrollerProperties &sp)
    : d(new QtScrollerPropertiesPrivate(*sp.d))
{
}

/*!
    Assigns \a sp to these scroller properties and returns a reference to these scroller properties.
*/
QtScrollerProperties &QtScrollerProperties::operator=(const QtScrollerProperties &sp)
{
    *d.data() = *sp.d.data();
    return *this;
}

/*!
    Destroys the scroller properties.
*/
QtScrollerProperties::~QtScrollerProperties()
{
}

/*!
    Returns true if these scroller properties are equal to \a sp; otherwise returns false.
*/
bool QtScrollerProperties::operator==(const QtScrollerProperties &sp) const
{
    return *d.data() == *sp.d.data();
}

/*!
    Returns true if these scroller properties are different from \a sp; otherwise returns false.
*/
bool QtScrollerProperties::operator!=(const QtScrollerProperties &sp) const
{
    return !(*d.data() == *sp.d.data());
}

bool QtScrollerPropertiesPrivate::operator==(const QtScrollerPropertiesPrivate &p) const
{
    bool same = true;
    same &= (mousePressEventDelay == p.mousePressEventDelay);
    same &= (dragStartDistance == p.dragStartDistance);
    same &= (dragVelocitySmoothingFactor == p.dragVelocitySmoothingFactor);
    same &= (axisLockThreshold == p.axisLockThreshold);
    same &= (scrollingCurve == p.scrollingCurve);
    same &= (decelerationFactor == p.decelerationFactor);
    same &= (minimumVelocity == p.minimumVelocity);
    same &= (maximumVelocity == p.maximumVelocity);
    same &= (maximumClickThroughVelocity == p.maximumClickThroughVelocity);
    same &= (acceleratingFlickMaximumTime == p.acceleratingFlickMaximumTime);
    same &= (acceleratingFlickSpeedupFactor == p.acceleratingFlickSpeedupFactor);
    same &= (snapPositionRatio == p.snapPositionRatio);
    same &= (snapTime == p.snapTime);
    same &= (overshootDragResistanceFactor == p.overshootDragResistanceFactor);
    same &= (overshootDragDistanceFactor == p.overshootDragDistanceFactor);
    same &= (overshootScrollDistanceFactor == p.overshootScrollDistanceFactor);
    same &= (overshootScrollTime == p.overshootScrollTime);
    same &= (hOvershootPolicy == p.hOvershootPolicy);
    same &= (vOvershootPolicy == p.vOvershootPolicy);
    same &= (frameRate == p.frameRate);
    return same;
}

/*!
     Sets the scroller properties for all new QtScrollerProperties objects to \a sp.

     Use this function to override the platform default properties returned by the default
     constructor. If you only want to change the scroller properties of a single scroller, use
     QtScroller::setScrollerProperties()

     \note Calling this function will not change the content of already existing
     QtScrollerProperties objects.

     \sa unsetDefaultScrollerProperties()
*/
void QtScrollerProperties::setDefaultScrollerProperties(const QtScrollerProperties &sp)
{
    if (!userDefaults)
        userDefaults = new QtScrollerPropertiesPrivate(*sp.d);
    else
        *userDefaults = *sp.d;
}

/*!
     Sets the scroller properties returned by the default constructor back to the platform default
     properties.

     \sa setDefaultScrollerProperties()
*/
void QtScrollerProperties::unsetDefaultScrollerProperties()
{
    delete userDefaults;
    userDefaults = 0;
}

/*!
    Query the \a metric value of the scroller properties.

    \sa setScrollMetric(), ScrollMetric
*/
QVariant QtScrollerProperties::scrollMetric(ScrollMetric metric) const
{
    switch (metric) {
    case MousePressEventDelay:          return d->mousePressEventDelay;
    case DragStartDistance:             return d->dragStartDistance;
    case DragVelocitySmoothingFactor:   return d->dragVelocitySmoothingFactor;
    case AxisLockThreshold:             return d->axisLockThreshold;
    case ScrollingCurve:                return QVariant::fromValue(d->scrollingCurve);
    case DecelerationFactor:            return d->decelerationFactor;
    case MinimumVelocity:               return d->minimumVelocity;
    case MaximumVelocity:               return d->maximumVelocity;
    case MaximumClickThroughVelocity:   return d->maximumClickThroughVelocity;
    case AcceleratingFlickMaximumTime:  return d->acceleratingFlickMaximumTime;
    case AcceleratingFlickSpeedupFactor:return d->acceleratingFlickSpeedupFactor;
    case SnapPositionRatio:             return d->snapPositionRatio;
    case SnapTime:                      return d->snapTime;
    case OvershootDragResistanceFactor: return d->overshootDragResistanceFactor;
    case OvershootDragDistanceFactor:   return d->overshootDragDistanceFactor;
    case OvershootScrollDistanceFactor: return d->overshootScrollDistanceFactor;
    case OvershootScrollTime:           return d->overshootScrollTime;
    case HorizontalOvershootPolicy:     return QVariant::fromValue(d->hOvershootPolicy);
    case VerticalOvershootPolicy:       return QVariant::fromValue(d->vOvershootPolicy);
    case FrameRate:                     return QVariant::fromValue(d->frameRate);
    case ScrollMetricCount:             break;
    }
    return QVariant();
}

/*!
    Set a specific value of the \a metric ScrollerMetric to \a value.

    \sa scrollMetric(), ScrollMetric
*/
void QtScrollerProperties::setScrollMetric(ScrollMetric metric, const QVariant &value)
{
    switch (metric) {
    case MousePressEventDelay:          d->mousePressEventDelay = value.toReal(); break;
    case DragStartDistance:             d->dragStartDistance = value.toReal(); break;
    case DragVelocitySmoothingFactor:   d->dragVelocitySmoothingFactor = qBound(qreal(0), value.toReal(), qreal(1)); break;
    case AxisLockThreshold:             d->axisLockThreshold = qBound(qreal(0), value.toReal(), qreal(1)); break;
    case ScrollingCurve:                d->scrollingCurve = value.value<QEasingCurve>(); break;
    case DecelerationFactor:            d->decelerationFactor = value.toReal(); break;
    case MinimumVelocity:               d->minimumVelocity = value.toReal(); break;
    case MaximumVelocity:               d->maximumVelocity = value.toReal(); break;
    case MaximumClickThroughVelocity:   d->maximumClickThroughVelocity = value.toReal(); break;
    case AcceleratingFlickMaximumTime:  d->acceleratingFlickMaximumTime = value.toReal(); break;
    case AcceleratingFlickSpeedupFactor:d->acceleratingFlickSpeedupFactor = value.toReal(); break;
    case SnapPositionRatio:             d->snapPositionRatio = qBound(qreal(0), value.toReal(), qreal(1)); break;
    case SnapTime:                      d->snapTime = value.toReal(); break;
    case OvershootDragResistanceFactor: d->overshootDragResistanceFactor = value.toReal(); break;
    case OvershootDragDistanceFactor:   d->overshootDragDistanceFactor = qBound(qreal(0), value.toReal(), qreal(1)); break;
    case OvershootScrollDistanceFactor: d->overshootScrollDistanceFactor = qBound(qreal(0), value.toReal(), qreal(1)); break;
    case OvershootScrollTime:           d->overshootScrollTime = value.toReal(); break;
    case HorizontalOvershootPolicy:     d->hOvershootPolicy = value.value<QtScrollerProperties::OvershootPolicy>(); break;
    case VerticalOvershootPolicy:       d->vOvershootPolicy = value.value<QtScrollerProperties::OvershootPolicy>(); break;
    case FrameRate:                     d->frameRate = value.value<QtScrollerProperties::FrameRates>(); break;
    case ScrollMetricCount:             break;
    }
}

/*!
    \enum QtScrollerProperties::FrameRates

    This enum describes the available frame rates used while dragging or scrolling.

    \value Fps60 60 frames per second
    \value Fps30 30 frames per second
    \value Fps20 20 frames per second
    \value Standard the default value is 60 frames per second (which corresponds to QAbstractAnimation).
*/

/*!
    \enum QtScrollerProperties::OvershootPolicy

    This enum describes the various modes of overshooting.

    \value OvershootWhenScrollable Overshooting is possible when the content is scrollable. This is the
                                   default.

    \value OvershootAlwaysOff Overshooting is never enabled, even when the content is scrollable.

    \value OvershootAlwaysOn Overshooting is always enabled, even when the content is not
                             scrollable.
*/

/*!
    \enum QtScrollerProperties::ScrollMetric

    This enum contains the different scroll metric types. When not indicated otherwise the
    setScrollMetric function expects a QVariant of type qreal.

    See the QtScroller documentation for further details of the concepts behind the different
    values.

    \value MousePressEventDelay This is the time a mouse press event is delayed when starting
    a flick gesture in \c{[s]}. If the gesture is triggered within that time, no mouse press or
    release is sent to the scrolled object. If it triggers after that delay the delayed
    mouse press plus a faked release event at global postion \c{QPoint(-QWIDGETSIZE_MAX,
    -QWIDGETSIZE_MAX)} is sent. If the gesture is canceled, then both the delayed mouse
    press plus the real release event are delivered.

    \value DragStartDistance This is the minimum distance the touch or mouse point needs to be
    moved before the flick gesture is triggered in \c m.

    \value DragVelocitySmoothingFactor A value that describes to which extent new drag velocities are
    included in the final scrolling velocity.  This value should be in the range between \c 0 and
    \c 1.  The lower the value, the more smoothing is applied to the dragging velocity.

    \value AxisLockThreshold Restricts the movement to one axis if the movement is inside an angle
    around the axis. The threshold must be in the range \c 0 to \c 1.

    \value ScrollingCurve The QEasingCurve used when decelerating the scrolling velocity after an
    user initiated flick. Please note that this is the easing curve for the positions, \bold{not}
    the velocity: the default is QEasingCurve::OutQuad, which results in a linear decrease in
    velocity (1st derivative) and a constant deceleration (2nd derivative).

    \value DecelerationFactor This factor influences how long it takes the scroller to decelerate
    to 0 velocity. The actual value depends on the chosen ScrollingCurve. For most
    types the value should be in the range from \c 0.1 to \c 2.0

    \value MinimumVelocity The minimum velocity that is needed after ending the touch or releasing
    the mouse to start scrolling in \c{m/s}.

    \value MaximumVelocity This is the maximum velocity that can be reached in \c{m/s}.

    \value MaximumClickThroughVelocity This is the maximum allowed scroll speed for a click-through
    in \c{m/s}. This means that a click on a currently (slowly) scrolling object will not only stop
    the scrolling but the click event will also be delivered to the UI control. This is
    useful when using exponential-type scrolling curves.

    \value AcceleratingFlickMaximumTime This is the maximum time in \c seconds that a flick gesture
    can take to be recognized as an accelerating flick. If set to zero no such gesture is
    detected. An "accelerating flick" is a flick gesture executed on an already scrolling object.
    In such cases the scrolling speed is multiplied by AcceleratingFlickSpeedupFactor in order to
    accelerate it.

    \value AcceleratingFlickSpeedupFactor The current speed is multiplied by this number if an
    accelerating flick is detected. Should be \c{>= 1}.

    \value SnapPositionRatio This is the distance that the user must drag the area beween two snap
    points in order to snap it to the next position. \c{0.33} means that the scroll must only
    reach one third of the distance between two snap points to snap to the next one. The ratio must
    be between \c 0 and \c 1.

    \value SnapTime This is the time factor for the scrolling curve. A lower value means that the
    scrolling will take longer. The scrolling distance is independet of this value.

    \value OvershootDragResistanceFactor This value is the factor between the mouse dragging and
    the actual scroll area movement (during overshoot). The factor must be between \c 0 and \c 1.

    \value OvershootDragDistanceFactor This is the maximum distance for overshoot movements while
    dragging. The actual overshoot distance is calculated by multiplying this value with the
    viewport size of the scrolled object. The factor must be between \c 0 and \c 1.

    \value OvershootScrollDistanceFactor This is the maximum distance for overshoot movements while
    scrolling. The actual overshoot distance is calculated by multiplying this value with the
    viewport size of the scrolled object. The factor must be between \c 0 and \c 1.

    \value OvershootScrollTime This is the time in \c seconds that is used to play the
    complete overshoot animation.

    \value HorizontalOvershootPolicy This is the horizontal overshooting policy (see OvershootPolicy).

    \value VerticalOvershootPolicy This is the horizontal overshooting policy (see OvershootPolicy).

    \value FrameRate This is the frame rate which should be used while dragging or scrolling.
    QtScroller uses a QAbstractAnimation timer internally to sync all scrolling operations to other
    animations that might be active at the same time.  If the standard value of 60 frames per
    second is too fast, it can be lowered with this setting,
    while still being in-sync with QAbstractAnimation. Please note that only the values of the
    FrameRates enum are allowed here.

    \value ScrollMetricCount This is always the last entry.
*/

