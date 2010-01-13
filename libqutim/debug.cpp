/****************************************************************************
 *  debug.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "debug.h"

namespace qutim_sdk_0_3
{
	typedef QMap<const QMetaObject *, QByteArray> DebugMap;
	Q_GLOBAL_STATIC(DebugMap, modules)

	QDebug debug_helper(qptrdiff ptr, DebugLevel level, QtMsgType type)
	{
		// TODO: Ability for disabling plugin's debug, check of level
		if (const QMetaObject *meta = reinterpret_cast<const QMetaObject *>(ptr)) {
			DebugMap::iterator it = modules()->find(meta);
			if (it == modules()->end()) {
				QByteArray name = metaInfo(meta, "DebugName");
				if (name.isEmpty())
					name = meta->className();
				name = name.trimmed();
				name.prepend("[");
				name.append("]:");
				it = modules()->insert(meta, name);
			}
			return (QDebug(type) << it.value().constData());
		} else {
			return (QDebug(type) << "[Core]:");
		}
	}
}
