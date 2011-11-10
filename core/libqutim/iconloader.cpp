/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "iconloader.h"
#include "objectgenerator.h"
#include "servicemanager.h"
#include <QPointer>
#include <QDebug>

namespace qutim_sdk_0_3
{
class IconWrapperPrivate
{
public:
	IconWrapperPrivate() : inited(false)
	{
	}

	inline void init() { if (!inited) init_helper(); }
	void init_helper();
	inline int size() { init(); return wrappers.size() + !self.isNull(); }
	inline IconWrapper *operator [](int i)
	{ init(); if (i == wrappers.size()) return self; else return wrappers.at(i); }
	
	bool inited;
	ServicePointer<IconLoader> self;
	QList<IconWrapper*> wrappers;
};

void IconWrapperPrivate::init_helper()
{
	if (!ObjectGenerator::isInited()) {
//		Q_ASSERT(!"Core is not loaded yet")
		return;
	}
	inited = true;
	const GeneratorList gens = ObjectGenerator::module<IconWrapper>();
	for (int i = 0; i < gens.size(); ++i) {
		if (gens[i]->extends<IconLoader>())
			continue;
		wrappers << gens[i]->generate<IconWrapper>();
	}
	wrappers.removeAll(NULL);
}

Q_GLOBAL_STATIC(IconWrapperPrivate, iconWrapper)

IconWrapper::~IconWrapper()
{
}

IconLoader::IconLoader()
{
}

IconLoader::~IconLoader()
{
}

QIcon IconLoader::loadIcon(const QString &name)
{
	QIcon result;
	IconWrapperPrivate &p = *iconWrapper();
	for (int i = 0; i < p.size() && result.isNull(); ++i)
		result = p[i]->doLoadIcon(name);
	return result;
}

QMovie *IconLoader::loadMovie(const QString &name)
{
	QMovie *result = 0;
	IconWrapperPrivate &p = *iconWrapper();
	for (int i = 0; i < p.size() && !result; ++i)
		result = p[i]->doLoadMovie(name);
	return result;
}

QString IconLoader::iconPath(const QString &name, uint iconSize)
{
	QString result;
	IconWrapperPrivate &p = *iconWrapper();
	for (int i = 0; i < p.size() && result.isNull(); ++i)
		result = p[i]->doIconPath(name, iconSize);
	return result;
}

QString IconLoader::moviePath(const QString &name, uint iconSize)
{
	QString result;
	IconWrapperPrivate &p = *iconWrapper();
	for (int i = 0; i < p.size() && result.isNull(); ++i)
		result = p[i]->doMoviePath(name, iconSize);
	return result;
}

void IconLoader::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}
}

