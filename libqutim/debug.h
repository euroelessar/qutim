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

#if !(defined(LIBQUTIM_LIBRARY) || defined(QUTIM_CORE))
extern qptrdiff qutim_plugin_id();
#endif

namespace qutim_sdk_0_3
{
	enum DebugLevel
	{
		DebugInfo = 0,
		DebugVerbose,
		DebugVeryVerbose
#if 1
		,
		Info = DebugInfo,
		Verbose = DebugVerbose,
		VeryVerbose = DebugVeryVerbose
#endif
	};

	LIBQUTIM_EXPORT QDebug debug_helper(qptrdiff, DebugLevel, QtMsgType);
//	LIBQUTIM_EXPORT qptrdiff debug_area_helper(const char *str);
	LIBQUTIM_EXPORT void debugClearConfig();
    
//#ifndef QUTIM_DEBUG_AREA
//#if defined (LIBQUTIM_LIBRARY)
//# define QUTIM_DEBUG_AREA "libqutim"
//#elif defined (QUTIM_CORE)
//# define QUTIM_DEBUG_AREA "core"
//#else
//# define QUTIM_DEBUG_AREA ""
//#endif
//	
//	inline qptrdiff debug_area(const char *str)
//	{
//		static qptrdiff area = debug_area_helper(str);
//		return area;
//	}

#if defined(LIBQUTIM_LIBRARY) || defined(QUTIM_CORE)
	inline QDebug debug(DebugLevel level = Info)
	{ return debug_helper(0, level, QtDebugMsg); }
	inline QDebug warning(DebugLevel level = Info)
	{ return debug_helper(0, level, QtWarningMsg); }
	inline QDebug critical(DebugLevel level = Info)
	{ return debug_helper(0, level, QtCriticalMsg); }
	inline QDebug fatal(DebugLevel level = Info)
	{ return debug_helper(0, level, QtFatalMsg); }
#else
	inline QDebug debug(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtDebugMsg); }
	inline QDebug warning(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtWarningMsg); }
	inline QDebug critical(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtCriticalMsg); }
	inline QDebug fatal(DebugLevel level = Info)
	{ return debug_helper(qutim_plugin_id(), level, QtFatalMsg); }
#endif
}
#endif // DEBUG_H
