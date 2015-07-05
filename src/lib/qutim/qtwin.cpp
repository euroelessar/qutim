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
#include "qtwin.h"
#ifdef Q_WS_WIN
# include "../3rdparty/qtdwm/qtdwm.h"
#endif
#include <QColor>
#include <QPalette>
#include <QApplication>

namespace qutim_sdk_0_3 {

	namespace QtWin {

		bool isCompositionEnabled()
		{
#ifdef Q_WS_WIN
			return QtDWM::isCompositionEnabled();
#else
			return false;
#endif
		}

		QColor colorizationColor()
		{
#ifdef Q_WS_WIN
			return QtDWM::colorizationColor();
#else
			return QApplication::palette().window().color();
#endif
		}

		bool enableBlurBehindWindow ( QWidget* widget, bool enable )
		{
#ifdef Q_WS_WIN
			return QtDWM::enableBlurBehindWindow(widget,enable);
#else
			Q_UNUSED(widget);
			Q_UNUSED(enable);
			return false;
#endif
		}

		bool extendFrameIntoClientArea ( QWidget* widget, int left, int top, int right, int bottom )
		{
#ifdef Q_WS_WIN
			return QtDWM::extendFrameIntoClientArea(widget,left,top,right,bottom);
#else
			Q_UNUSED(widget);
			Q_UNUSED(left);
			Q_UNUSED(top);
			Q_UNUSED(right);
			Q_UNUSED(bottom);
			return false;
#endif
		}

	}
}

