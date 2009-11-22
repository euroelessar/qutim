#include "plugin_p.h"
#include <QtCore/QCoreApplication>

namespace qutim_sdk_0_3
{
	ExtensionInfoData::ExtensionInfoData() : gen(0)
	{
		name.setContext("Plugin");
		description.setContext("Plugin");
	}

	ExtensionInfoData::ExtensionInfoData(const ExtensionInfoData &o)
			: QSharedData(o), name(o.name), description(o.description), gen(o.gen), icon(o.icon), plugin(o.plugin)
	{
		name.setContext("Plugin");
		description.setContext("Plugin");
	}

	ExtensionInfo::ExtensionInfo(const char *name, const char *description, const ObjectGenerator *generator, ExtensionIcon icon)
	{
		d = new ExtensionInfoData;
		setName(name);
		setDescription(description);
		setGenerator(generator);
		setIcon(icon);
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

	ExtensionInfo &ExtensionInfo::setName(const char *name)
	{
		d->name = name;
		return *this;
	}

	ExtensionInfo &ExtensionInfo::setDescription(const char *description)
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

	ExtensionInfo::Data *ExtensionInfo::data()
	{
		return const_cast<Data *>(d.constData());
	}
}
