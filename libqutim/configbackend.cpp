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

	ConfigBackend::ConfigBackend()
	{
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
}
