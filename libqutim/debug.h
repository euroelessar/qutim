/****************************************************************************
 *  debug.h
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

#ifndef DEBUG_H
#define DEBUG_H

// Include QDebug, because we want to redefine some symbols
#include <QDebug>
#include "libqutim_global.h"

#if defined(LIBQUTIM_LIBRARY) || defined(QUTIM_CORE)
qptrdiff qutim_plugin_id() { return 0; }
#else
extern qptrdiff qutim_plugin_id();
#endif

namespace qutim_sdk_0_3
{
	enum DebugLevel
	{
		VeryVerbose,
		Verbose,
		Info
	};

	LIBQUTIM_EXPORT QDebug debug_helper(qptrdiff, DebugLevel, QtMsgType);

	inline QDebug debug(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtDebugMsg); }
	inline QDebug warning(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtWarningMsg); }
	inline QDebug critical(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtCriticalMsg); }
	inline QDebug fatal(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtFatalMsg); }
}
#endif // DEBUG_H
