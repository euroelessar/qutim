/****************************************************************************
 *  objectgenerator.cpp
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

#include "objectgenerator_p.h"
#include <QtCore/QVariant>

namespace qutim_sdk_0_3
{
	ObjectGenerator::ObjectGenerator() : d_ptr(new ObjectGeneratorPrivate)
	{
	}

	ObjectGenerator::ObjectGenerator(ObjectGeneratorPrivate &priv) : d_ptr(&priv)
	{
	}

	ObjectGenerator::~ObjectGenerator()
	{
	}

	ObjectGenerator *ObjectGenerator::addProperty(const QByteArray &name, const QVariant &value)
	{
		Q_D(ObjectGenerator);
		int index = d->names.indexOf(name);
		if (index != -1) {
			d->values[index] = value;
		} else {
			d->names.append(name);
			d->values.append(value);
		}
		return this;
	}

	bool ObjectGenerator::extends(const QMetaObject *super) const
	{
		const QMetaObject *meta = metaObject();
		while (meta && meta != super)
			meta = meta->superClass();
		return super && meta == super;
	}

	QObject *ObjectGenerator::generateHelper2() const
	{
		Q_D(const ObjectGenerator);
		if (QObject *obj = generateHelper()) {
			for (int i = 0; i < d->names.size(); i++)
				obj->setProperty(d->names.at(i), d->values.at(i));
			if (d->info.data())
				obj->setProperty("extensioninfo", QVariant::fromValue(d->info));
			return obj;
		}
		return NULL;
	}
}
