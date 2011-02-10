/****************************************************************************
 *  kineticscroller.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "kineticscroller.h"
#include <qtscroller.h>
#include <qutim/debug.h>

namespace Core {

using namespace qutim_sdk_0_3;

KineticScroller::KineticScroller()
{
	debug() << Q_FUNC_INFO;
}

void KineticScroller::enableScrolling(QObject *widget)
{
	QtScroller::grabGesture(widget, QtScroller::LeftMouseButtonGesture);
}

} // namespace Core
