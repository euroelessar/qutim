/****************************************************************************
 *  icon.cpp
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

#include "icon.h"
#include "iconbackend_p.h"
#include "iconloader.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	struct IconPrivate
	{
		QList<IconWrapper*> wrappers;
	};
	static IconPrivate *p = NULL;

	void createIconPrivate()
	{
		if (p || !isCoreInited()) return;
		p = new IconPrivate;
		GeneratorList gens = moduleGenerators<IconWrapper>();
		foreach (const ObjectGenerator *gen, gens)
			p->wrappers << gen->generate<IconWrapper>();
	}

	QIcon loadIcon(const QString &name)
	{
		if (!p) createIconPrivate();
		QIcon result;
		if (p) {
			for (int i = 0; i < p->wrappers.size(); i++) {
				result = p->wrappers[i]->getIcon(name);
				if (!result.isNull())
					return result;
			}
			if (IconLoader *loader = IconLoader::instance())
				result = loader->loadIcon(name);
		}
		return result;
	}

	Icon::Icon(const QString &name) : QIcon(loadIcon(name))
	{
	}

	Icon::Icon(const QIcon &icon) : QIcon(icon)
	{
	}
}
