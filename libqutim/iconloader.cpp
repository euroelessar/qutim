/****************************************************************************
 *  iconloader.cpp
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

#include "iconloader.h"
#include "objectgenerator.h"
#include <QPointer>
#include <QDebug>

namespace qutim_sdk_0_3
{
	IconLoader::IconLoader()
	{
	}

	IconLoader::~IconLoader()
	{
	}

	IconLoader *IconLoader::instance()
	{
		static QPointer<IconLoader> self;
		if(self.isNull() && isCoreInited())
			self = qobject_cast<IconLoader*>(getService("IconLoader"));
		return self.data();
	}

	void IconLoader::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}
