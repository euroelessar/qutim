#include "iconloader.h"
#include "objectgenerator.h"
#include <QPointer>

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
		static bool is_possible = true;
		if(is_possible && self.isNull() && isCoreInited())
		{
			GeneratorList exts = moduleGenerators<IconLoader>();
			if(exts.isEmpty())
			{
				is_possible = false;
				return NULL;
			}
			self = exts.first()->generate<IconLoader>();
		}
		return self.data();
	}
}
