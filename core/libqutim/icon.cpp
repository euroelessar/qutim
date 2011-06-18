/****************************************************************************
 *  icon.cpp
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

#include "icon.h"
#include "iconbackend_p.h"
#include "iconloader.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	Icon::Icon(const QString &name) : QIcon(IconLoader::loadIcon(name))
	{
	}

	Icon::Icon(const QIcon &icon) : QIcon(icon)
	{
	}

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
	QString Icon::name() const
	{
		return QIcon::name();
	}
#else
# define IconNameHook 2
	struct IconPrivateHook
	{
		QIconEngineV2 *engine;
		QAtomicInt ref;
		int serialNum;
		int detach_no;
		int engine_version;
	};
	
	QString Icon::name() const
	{
		QString name;
		DataPtr data = const_cast<Icon*>(this)->data_ptr();
		IconPrivateHook *p = reinterpret_cast<IconPrivateHook*>(data);
		if (!p || !p->engine || p->engine_version < 2)
			return name;
		p->engine->virtual_hook(IconNameHook, &name);
		return name;
	}
#endif
}
