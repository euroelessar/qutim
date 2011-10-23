/****************************************************************************
 *  objectgenerator_p.h
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

#ifndef OBJECTGENERATOR_P_H
#define OBJECTGENERATOR_P_H

#include "plugin_p.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	class ObjectGeneratorPrivate
	{
	public:
		QList<QByteArray> names;
		QList<QVariant>   values;
		ExtensionInfo info;
		ObjectGenerator::Ptr pointer;
	};
}

#endif // OBJECTGENERATOR_P_H
