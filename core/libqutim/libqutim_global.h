/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef LIBQUTIM_GLOBAL_H
#define LIBQUTIM_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QObject>

#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
# error Incompatible Qt library, need 4.6.0 at least
#endif

#ifdef QT_VISIBILITY_AVAILABLE
# define LIBQUTIM_NO_EXPORT __attribute__ ((visibility("hidden")))
#else
# define LIBQUTIM_NO_EXPORT
#endif

#if defined(LIBQUTIM_LIBRARY)
#  define LIBQUTIM_EXPORT Q_DECL_EXPORT
#else
#  define LIBQUTIM_EXPORT Q_DECL_IMPORT
#endif

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR) || defined(Q_WS_MAEMO_5)
#	ifndef QUTIM_MOBILE_UI
#		define QUTIM_MOBILE_UI
#	endif
#endif

namespace qutim_sdk_0_3
{
	LIBQUTIM_EXPORT void centerizeWidget(QWidget *widget); //move widget to center of current screen
}

#endif // LIBQUTIM_GLOBAL_H

