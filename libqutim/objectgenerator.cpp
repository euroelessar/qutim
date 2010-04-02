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

#include "actiongenerator_p.h"
#include <QtCore/QVariant>
#include <QtCore/QCoreApplication>

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
			if (dynamic_cast<const ActionGenerator*>(this) && qobject_cast<QAction*>(obj)) {
				const ActionGeneratorPrivate *p = static_cast<const ActionGeneratorPrivate*>(d);
				const QList<QPointer<QObject> > &handlers = p->handlers;
				ActionCreatedEvent event(static_cast<QAction*>(obj), const_cast<ActionGenerator*>(static_cast<const ActionGenerator*>(this)));
				for (int i = 0; i < handlers.size(); i++) {
					if (QObject *handler = handlers.at(i))
						qApp->sendEvent(handler, &event);
				}
			}
			return obj;
		}
		return NULL;
	}

	ExtensionInfo ObjectGenerator::info() const
	{
		return d_func()->info;
	}

	void ObjectGenerator::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}
