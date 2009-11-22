/****************************************************************************
 *  objectgenerator.cpp
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

#include "objectgenerator_p.h"
#include <QtCore/QVariant>

namespace qutim_sdk_0_3
{
	ObjectGenerator::ObjectGenerator() : p(new ObjectGeneratorPrivate)
	{
	}

	ObjectGenerator::~ObjectGenerator()
	{
	}

	ObjectGenerator *ObjectGenerator::addProperty(const QByteArray &name, const QVariant &value)
	{
		int index = p->names.indexOf(name);
		if (index != -1) {
			p->values[index] = value;
		} else {
			p->names.append(name);
			p->values.append(value);
		}
		return this;
	}

	QObject *ObjectGenerator::generateHelper2() const
	{
		if (QObject *obj = generateHelper()) {
			for (int i = 0; i < p->names.size(); i++)
				obj->setProperty(p->names.at(i), p->values.at(i));
			if (p->info.data())
				obj->setProperty("extensioninfo", QVariant::fromValue(p->info));
			return obj;
		}
		return NULL;
	}
}
