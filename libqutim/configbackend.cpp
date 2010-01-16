/****************************************************************************
 *  configbackend.cpp
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

#include "configbackend.h"
#include <QFile>

namespace qutim_sdk_0_3
{
	bool ConfigEntry::remove()
	{
//		if(!ref)
//		{
			value.clear();
			map.clear();
			array.clear();
			return (dirty = deleted = true);
//		}
//		bool result = dirty = deleted = true;
//		if(type & Value)
//			value.clear();
//		if(type & Map)
//		{
//			for(EntryMap::iterator it = map.begin(); it != map.end();)
//			{
//				if(it.value()->remove())
//					map.erase(it);
//				else
//				{
//					result = false;
//					it++;
//				}
//			}
//		}
//		if(type & Array)
//		{
//			for(EntryArray::iterator it = array.begin(); it != array.end();)
//			{
//				if((*it)->remove())
//					array.erase(it);
//				else
//				{
//					result = false;
//					it++;
//				}
//			}
//		}
//		return result;
	}

	struct ConfigBackendPrivate
	{
		QByteArray extension;
	};

	ConfigBackend::ConfigBackend() : p(new ConfigBackendPrivate)
	{
	}

	ConfigBackend::~ConfigBackend()
	{
	}

	QByteArray ConfigBackend::backendName() const
	{
		if(p->extension.isNull())
			p->extension = metaInfo(metaObject(), "Extension");
		return p->extension;
	}

	QIODevice *ConfigBackend::openDefault(const QString &file, QIODevice::OpenMode mode)
	{
		// TODO: if relative prepend profile config path
		QFile *device = new QFile(file);
		if(!device->open(mode))
		{
			delete device;
			return 0;
		}
		return device;
	}

	void ConfigBackend::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}
