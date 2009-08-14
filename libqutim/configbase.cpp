#include "configbase_p.h"
#include "cryptoservice.h"
#include <QStringList>

namespace qutim_sdk_0_3
{
	ConfigBase::ConfigBase()
	{
	}

	ConfigBase::~ConfigBase()
	{
	}

	Config::Config(const QString &file, OpenFlags flags, const QString &backend)
	{
	}

	QString Config::name() const
	{
	}

	QStringList Config::groupList() const
	{
	}

	bool Config::hasGroup(const QString &name) const
	{
	}

	const ConfigGroup Config::group(const QString &name) const
	{
	}

	ConfigGroup Config::group(const QString &name)
	{
	}

	void Config::removeGroup(const QString &name)
	{
	}

	void Config::sync()
	{
	}

	ConfigGroup::ConfigGroup()
	{
	}

	QString ConfigGroup::name() const
	{
	}

	bool ConfigGroup::isValid() const
	{
	}

	bool ConfigGroup::isMap() const
	{
	}

	bool ConfigGroup::isArray() const
	{
	}

	bool ConfigGroup::isValue() const
	{
	}

	QStringList ConfigGroup::groupList() const
	{
		QStringList list;
		if(p->entry->type & ConfigEntry::Map)
		{
		}
		return list;
	}

	bool ConfigGroup::hasGroup(const QString &name) const
	{
		if(p->entry->type & ConfigEntry::Map)
			return p->entry->map.contains(name);
		return false;
	}

	const ConfigGroup ConfigGroup::group(const QString &name) const
	{
	}

	ConfigGroup ConfigGroup::group(const QString &name)
	{
		ConfigGroup group;
		return group;
	}

	void ConfigGroup::removeGroup(const QString &name)
	{
		if(p->entry->type & ConfigEntry::Map)
			p->entry->map.remove(name);
	}

	int ConfigGroup::arraySize() const
	{
		if(p->entry->type & ConfigEntry::Array)
			return p->entry->array.size();
		return 0;
	}

	const ConfigGroup ConfigGroup::at(int index) const
	{
		ConfigGroup group;
		if(p->entry->type & ConfigEntry::Array)
		{
			Q_ASSERT_X(index >= 0 && index < p->entry->array.size(), "ConfigGroup::at", "index out of range");
			group.p = new ConfigGroupPrivate;
			group.p->name = QString::number(index);
			group.p->parent = p;
			group.p->config = p->config;
			group.p->entry = p->entry->array[index];
		}
		return group;
	}

	ConfigGroup ConfigGroup::at(int index)
	{
		ConfigGroup group;
		if(p->entry->type & ConfigEntry::Array)
		{
			Q_ASSERT_X(index >= 0, "ConfigGroup::at", "index out of range");
			if(p->entry->array.size() <= index)
				p->entry->array.resize(index + 1);
			group.p = new ConfigGroupPrivate;
			group.p->name = QString::number(index);
			group.p->parent = p;
			group.p->config = p->config;
			group.p->entry = p->entry->array[index];
		}
		return group;
	}

	void ConfigGroup::removeAt(int index)
	{
		if(p->entry->type & ConfigEntry::Array)
		{
			Q_ASSERT_X(index >= 0 && index < p->entry->array.size(), "ConfigGroup::removeAt", "index out of range");
			p->entry->array.remove(index);
		}
	}

	ConfigGroup ConfigGroup::parent()
	{
		ConfigGroup group;
		group.p = p->parent;
		return group;
	}

	const ConfigGroup ConfigGroup::parent() const
	{
		ConfigGroup group;
		group.p = p->parent;
		return group;
	}

	Config ConfigGroup::config()
	{
		Config config;
		config.p = p->config;
		return config;
	}

	const Config ConfigGroup::config() const
	{
		Config config;
		config.p = p->config;
		return config;
	}

	QVariant ConfigGroup::readEntry(const QString &key, const QVariant &def, Config::ValueFlag type) const
	{
		QVariant result; // someway get value
		if(type & Config::Crypted)
			return CryptoService::decrypt(result.toByteArray());
		return result;
	}

	void ConfigGroup::writeEntry(const QString &key, const QVariant &value, Config::ValueFlag type)
	{
		QVariant tmp = (type & Config::Crypted) ? CryptoService::crypt(value) : value;
		// someway write value
	}

	void ConfigGroup::sync()
	{
		config().sync();
	}
}
