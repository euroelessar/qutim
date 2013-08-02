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
    LIBQUTIM_EXPORT QtMsgType debug_level(const char *name);

#undef qDebug
#undef qWarning
#undef qCritical
#undef qFatal
#define qDebug QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, QUTIM_PLUGIN_NAME).debug
#define qWarning QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, QUTIM_PLUGIN_NAME).warning
#define qCritical QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, QUTIM_PLUGIN_NAME).critical
#define qFatal QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, QUTIM_PLUGIN_NAME).fatal
}
#endif // DEBUG_H

