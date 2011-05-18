/****************************************************************************
 *  libqutim_global.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
