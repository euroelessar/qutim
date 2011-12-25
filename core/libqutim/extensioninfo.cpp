/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "plugin_p.h"
#include "objectgenerator_p.h"
#include <QtCore/QCoreApplication>

namespace qutim_sdk_0_3
{
	ExtensionInfoData::ExtensionInfoData() : gen(0)
	{
	}

	ExtensionInfoData::ExtensionInfoData(const ExtensionInfoData &o)
			: QSharedData(o), name(o.name), description(o.description), gen(o.gen), icon(o.icon), plugin(o.plugin)
	{
	}

	ExtensionInfo::ExtensionInfo(const LocalizedString &name, const LocalizedString &description, const ObjectGenerator *generator, ExtensionIcon icon)
	{
		d = new ExtensionInfoData;
		setName(name);
		setDescription(description);
		setIcon(icon);
		setGenerator(generator);
	}

	ExtensionInfo::ExtensionInfo(const ExtensionInfo &other) : d(other.d)
	{
	}

	ExtensionInfo::~ExtensionInfo()
	{
	}

	ExtensionInfo &ExtensionInfo::operator =(const ExtensionInfo &other)
	{
		d = other.d;
		return *this;
	}

	ExtensionInfo &ExtensionInfo::setName(const LocalizedString &name)
	{
		d->name = name;
		return *this;
	}

	ExtensionInfo &ExtensionInfo::setDescription(const LocalizedString &description)
	{
		d->description = description;
		return *this;
	}

	ExtensionInfo &ExtensionInfo::setIcon(const ExtensionIcon &icon)
	{
		d->icon = icon;
		return *this;
	}

	ExtensionInfo &ExtensionInfo::setGenerator(const ObjectGenerator *generator)
	{
		d->gen = generator;
		// FIXME: Set should be done only at final stage somewhere in ModuleManager
		if (d->gen)
			const_cast<ObjectGenerator*>(d->gen)->data()->info = *this;
		return *this;
	}

	QList<PersonInfo> ExtensionInfo::authors() const
	{
		return d->plugin.authors();
	}

	LocalizedString ExtensionInfo::name() const
	{
		return d->name;
	}

	LocalizedString ExtensionInfo::description() const
	{
		return d->description;
	}

	const ObjectGenerator *ExtensionInfo::generator() const
	{
		return d->gen;
	}

	ExtensionIcon ExtensionInfo::icon() const
	{
		return d->icon;
	}

	ExtensionInfo::Data *ExtensionInfo::data() const
	{
		return const_cast<Data *>(d.constData());
	}
}

