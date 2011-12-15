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

#ifndef QTSCROLLERPROPERTIES_P_H
#define QTSCROLLERPROPERTIES_P_H

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

#include <QPointF>
#include <QEasingCurve>
#include "qtscrollerproperties.h"

class QtScrollerPropertiesPrivate
{
public:
    static QtScrollerPropertiesPrivate *defaults();

    bool operator==(const QtScrollerPropertiesPrivate &) const;

    qreal mousePressEventDelay;
    qreal dragStartDistance;
    qreal dragVelocitySmoothingFactor;
    qreal axisLockThreshold;
    QEasingCurve scrollingCurve;
    qreal decelerationFactor;
    qreal minimumVelocity;
    qreal maximumVelocity;
    qreal maximumClickThroughVelocity;
    qreal acceleratingFlickMaximumTime;
    qreal acceleratingFlickSpeedupFactor;
    qreal snapPositionRatio;
    qreal snapTime;
    qreal overshootDragResistanceFactor;
    qreal overshootDragDistanceFactor;
    qreal overshootScrollDistanceFactor;
    qreal overshootScrollTime;

    QtScrollerProperties::OvershootPolicy hOvershootPolicy;
    QtScrollerProperties::OvershootPolicy vOvershootPolicy;
    QtScrollerProperties::FrameRates frameRate;
};

#endif // QTSCROLLERPROPERTIES_P_H

