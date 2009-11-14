/****************************************************************************
 *  icon.cpp
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

#include "icon.h"
#include "iconbackend_p.h"
#include "iconloader.h"

namespace qutim_sdk_0_3
{
	inline QIcon loadIcon(const QString &name)
	{
		if(IconLoader *loader = IconLoader::instance())
			return loader->loadIcon(name);
		return QIcon();
	}

	Icon::Icon(const QString &name) : QIcon(loadIcon(name))
	{
	}

	Icon::Icon(const QIcon &icon) : QIcon(icon)
	{
	}
}
