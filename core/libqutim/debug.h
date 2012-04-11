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

#ifndef DEBUG_H
#define DEBUG_H

// Include QDebug, because we want to redefine some symbols
#include <QDebug>
#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	enum DebugLevel
	{
		DebugInfo = 0,
		DebugVerbose,
		DebugVeryVerbose
	};

	LIBQUTIM_EXPORT QDebug debug_helper(quint64, DebugLevel, QtMsgType);
	LIBQUTIM_EXPORT void debugAddPluginId(quint64, const QMetaObject *meta);
	LIBQUTIM_EXPORT void debugClearConfig();

#ifndef QUTIM_PLUGIN_ID
	inline QDebug debug(DebugLevel level = DebugInfo)
	{ return debug_helper(0, level, QtDebugMsg); }
	inline QDebug warning(DebugLevel level = DebugInfo)
	{ return debug_helper(0, level, QtWarningMsg); }
	inline QDebug critical(DebugLevel level = DebugInfo)
	{ return debug_helper(0, level, QtCriticalMsg); }
	inline QDebug fatal(DebugLevel level = DebugInfo)
	{ return debug_helper(0, level, QtFatalMsg); }
#else
# define QUTIM_DEBUG_ID_CONVERT_HELPER(A) 0x ## A ## ULL
# define QUTIM_DEBUG_ID_CONVERT(A) QUTIM_DEBUG_ID_CONVERT_HELPER(A)
	inline quint64 qutim_plugin_id()
	{ return QUTIM_DEBUG_ID_CONVERT(QUTIM_PLUGIN_ID); }
	inline QDebug debug(DebugLevel level = DebugInfo)
	{ return debug_helper(qutim_plugin_id(), level, QtDebugMsg); }
	inline QDebug warning(DebugLevel level = DebugInfo)
	{ return debug_helper(qutim_plugin_id(), level, QtWarningMsg); }
	inline QDebug critical(DebugLevel level = DebugInfo)
	{ return debug_helper(qutim_plugin_id(), level, QtCriticalMsg); }
	inline QDebug fatal(DebugLevel level = DebugInfo)
	{ return debug_helper(qutim_plugin_id(), level, QtFatalMsg); }
# undef QUTIM_DEBUG_ID_CONVERT_HELPER
# undef QUTIM_DEBUG_ID_CONVERT
#endif
}
#endif // DEBUG_H

