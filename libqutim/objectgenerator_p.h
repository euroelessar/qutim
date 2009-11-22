#ifndef OBJECTGENERATOR_P_H
#define OBJECTGENERATOR_P_H

#include "plugin_p.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	struct ObjectGeneratorPrivate
	{
		QList<QByteArray> names;
		QList<QVariant>   values;
		ExtensionInfo info;
	};
}

#endif // OBJECTGENERATOR_P_H
