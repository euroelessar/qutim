#include "configbase_p.h"
#include "cryptoservice.h"
#include <QStringList>
#include <QSet>
#include <QFileInfo>

#include <QDebug>

namespace qutim_sdk_0_3
{
	QList<ConfigBackendInfo> ConfigPrivate::config_backends = QList<ConfigBackendInfo>();

//	ConfigStrongEntryList ConfigBase::getEntries() const
//	{
//		ConfigStrongEntryList result;
//		ConfigEntryList entries;
//		if(const ConfigGroup *group = dynamic_cast<const ConfigGroup *>(this))
//			entries = group->p->entries;
//		if(const Config *config = dynamic_cast<const Config *>(this))
//			entries = config->p->entries;
//		return result;
//	}

	ConfigBase::ConfigBase()
	{
	}

	ConfigBase::~ConfigBase()
	{
	}

	QExplicitlySharedDataPointer<ConfigBasePrivate> ConfigBase::get_p() const
	{
		if(const ConfigGroup *group = dynamic_cast<const ConfigGroup *>(this))
			return group->p;
		if(const Config *config = dynamic_cast<const Config *>(this))
			return config->p;
		return QExplicitlySharedDataPointer<ConfigBasePrivate>();
	}

	bool ConfigBase::isGroup() const
	{
		return dynamic_cast<const ConfigGroup *>(this) == this;
	}

	QStringList ConfigBase::groupList() const
	{
//		ConfigEntryList entries;
//		if(const ConfigGroup *group = dynamic_cast<const ConfigGroup *>(this))
//			entries = group->p->entries;
//		else
//			entries = static_cast<const Config *>(this)->p->entries;
		ConfigBasePrivatePtr p = get_p();
		QSet<QString> groups;
		for(int i = 0; i < p->entries.size(); i++)
		{
			ConfigEntry::Ptr entry = p->entries.at(i);
			if(entry->type & ConfigEntry::Map)
			{
				QStringList keys = entry->map.keys();
				for(int j = 0; j < keys.size(); j++)
					groups |= keys.at(j);
			}
		}
		return groups.toList();
	}

	bool ConfigBase::hasGroup(const QString &name) const
	{
//		ConfigEntryList entries;
//		if(const ConfigGroup *group = dynamic_cast<const ConfigGroup *>(this))
//			entries = group->p->entries;
//		else
//			entries = static_cast<const Config *>(this)->p->entries;
		ConfigBasePrivatePtr p = get_p();
		for(int i = 0; i < p->entries.size(); i++)
		{
			ConfigEntry::Ptr entry = p->entries.at(i);
			if((entry->type & ConfigEntry::Map) && entry->map.contains(name))
				return true;
		}
		return false;
	}

	const ConfigGroup ConfigBase::group(const QString &name) const
	{
//		ConfigEntryList entries;
//		if(const ConfigGroup *group = dynamic_cast<const ConfigGroup *>(this))
//			entries = group->p->entries;
//		else
//			entries = static_cast<const Config *>(this)->p->entries;
		ConfigBasePrivatePtr p = get_p();
		if(name.contains('/'))
		{
			QStringList names = name.split('/', QString::SkipEmptyParts);
			Q_ASSERT_X(!names.isEmpty(), "Config::group", "<censored>");
			ConfigGroup group = this->group(names.at(0));
			for(int i = 1; i < names.size(); i++)
				if(!names.at(i).isEmpty())
					group = group.group(names.at(i));
			if(isGroup())
				group.p->parent = dynamic_cast<const ConfigGroup *>(this)->p;
			else
				group.p->parent.reset();
			return group;
		}
		ConfigGroup group;
		group.p = new ConfigGroupPrivate;
		group.p->name = name;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map))
			{
				ConfigEntry::Ptr entry = p->entries.at(i);
				ConfigEntry::EntryMap::const_iterator it = entry->map.find(name);
				if(it != entry->map.end())
					group.p->entries.append(*it);
			}
		if(const ConfigGroup *me = dynamic_cast<const ConfigGroup *>(this))
		{
			group.p->parent = me->p;
			group.p->config = me->p->config;
		}
		else
		{
			const Config *me = static_cast<const Config *>(this);
			group.p->parent.reset();
			group.p->config = me->p;
		}
		return group;
	}

	ConfigGroup ConfigBase::group(const QString &name)
	{
//		ConfigEntryList entries;
//		if(const ConfigGroup *group = dynamic_cast<const ConfigGroup *>(this))
//			entries = group->p->entries;
//		else
//			entries = static_cast<const Config *>(this)->p->entries;
		ConfigBasePrivatePtr p = get_p();
		if(name.contains('/'))
		{
			//TODO: Optimize, split is too slow for this situation, use indexOf instead
			QStringList names = name.split('/', QString::SkipEmptyParts);
			Q_ASSERT_X(!names.isEmpty(), "Config::group", "<censored>");
			ConfigGroup group = this->group(names.at(0));
			for(int i = 1; i < names.size(); i++)
				if(!names.at(i).isEmpty())
					group = group.group(names.at(i));
			if(isGroup())
				group.p->parent = dynamic_cast<ConfigGroup *>(this)->p;
			else
				group.p->parent.reset();
			return group;
		}
		ConfigGroup group;
		group.p = new ConfigGroupPrivate;
		group.p->name = name;
		{
			ConfigEntry::Ptr entry = p->entries[0];
			ConfigEntry::EntryMap::const_iterator it;
			if((entry->type & ConfigEntry::Map) && ((it = entry->map.constFind(name)) != entry->map.constEnd()))
				group.p->entries.append(*it);
			else
			{
				if((entry->type & ConfigEntry::AnyGroup) == ConfigEntry::Array)
				{
					entry->array.clear();
					entry->type &= ~ConfigEntry::Array;
				}
				entry->type |= ConfigEntry::Map;
				ConfigEntry::Ptr new_entry(new ConfigEntry);
				entry->map.insert(name, new_entry);
				group.p->entries.append(new_entry);
			}
		}
		for(int i = 1; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map))
			{
				ConfigEntry::Ptr entry = p->entries.at(i);
				ConfigEntry::EntryMap::const_iterator it = entry->map.constFind(name);
				if(it != entry->map.end())
					group.p->entries.append(*it);
			}
		if(const ConfigGroup *me = dynamic_cast<const ConfigGroup *>(this))
		{
			group.p->parent = me->p;
			group.p->config = me->p->config;
		}
		else
		{
			const Config *me = static_cast<const Config *>(this);
			group.p->parent.reset();
			group.p->config = me->p;
		}
		return group;
	}

	void ConfigBase::removeGroup(const QString &name)
	{
//		ConfigEntryList entries;
//		if(const ConfigGroup *group = dynamic_cast<const ConfigGroup *>(this))
//			entries = group->p->entries;
//		else
//			entries = static_cast<const Config *>(this)->p->entries;
		ConfigBasePrivatePtr p = get_p();
		ConfigEntry::Ptr entry;
		if(p->entries.size() && (entry = p->entries.at(0)))
			entry->map.remove(name);
	}

	void config_add_file(QExplicitlySharedDataPointer<ConfigPrivate> &config, const QString &file)
	{
//		QFileInfo info(file);
		Q_UNUSED(config);
		Q_UNUSED(file);
	}

	Config::Config(const QString &file, OpenFlags flags, const QString &backend_)
	{
		if(file.isEmpty())
			return;
		QFileInfo info(file);
		if(info.isAbsolute())
		{
			ConfigEntry::Ptr entry;
			ConfigBackend *backend_ptr = 0;
			{
				QString backend = backend_;
				if(backend.isEmpty())
					backend = info.suffix();
				for(int i = 0; i < ConfigPrivate::config_backends.size(); i++)
				{
					if(ConfigPrivate::config_backends.at(i).first == backend)
						entry = (backend_ptr = ConfigPrivate::config_backends.at(i).second)->parse(file);
				}
				if(entry.isNull())
				{
					entry = (backend_ptr = ConfigPrivate::config_backends.at(0).second)->parse(file);
				}
			}
			if(!backend_ptr)
				return;
			p = new ConfigPrivate;
			ConfigEntryInfo entry_info = { entry, file, backend_ptr };
			p->root_entries << entry_info;
			p->entries << entry.toWeakRef();
			p->file = file;
		}
		else
		{
		}
	}
		
	Config::Config(const Config &other) : p(other.p)
	{
	}

	Config::Config(const QExplicitlySharedDataPointer<ConfigPrivate> &other) : p(other)
	{
	}

	Config::~Config()
	{
	}

	void Config::sync()
	{
		foreach(const ConfigEntryInfo &info, p->root_entries)
			if(info.backend && !info.file.isEmpty())
			{
//				qDebug() << "sync" << info.file;
				info.backend->generate(info.file, info.root);
			}
	}

	ConfigGroup::ConfigGroup()
	{
	}

	ConfigGroup::ConfigGroup(const ConfigGroup &other) : p(other.p)
	{
	}

	ConfigGroup::ConfigGroup(const QExplicitlySharedDataPointer<ConfigGroupPrivate> &other) : p(other)
	{
	}

	ConfigGroup::~ConfigGroup()
	{
	}

	QString ConfigGroup::name() const
	{
		return p->name;
	}

	bool ConfigGroup::isValid() const
	{
		return p.constData() && p->entries.size();
	}

	bool ConfigGroup::isMap() const
	{
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map))
				return true;
		return false;
	}

	bool ConfigGroup::isArray() const
	{
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Array))
				return true;
		return false;
	}

	bool ConfigGroup::isValue() const
	{
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Value))
				return true;
		return false;
	}

	int ConfigGroup::arraySize() const
	{
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Array))
				return p->entries.at(i).toStrongRef()->array.size();
		return 0;
	}

	const ConfigGroup ConfigGroup::at(int index) const
	{
		ConfigGroup group;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Array))
			{
				ConfigEntry::Ptr entry = p->entries.at(i);
				if(index < 0 || index >= entry->array.size())
					continue;
				if(!group.p)
				{
					group.p = new ConfigGroupPrivate;
					group.p->index = index;
					group.p->parent = p;
					group.p->config = p->config;
				}
				group.p->entries << entry->array[index];
			}
		return group;
	}

	ConfigGroup ConfigGroup::at(int index)
	{
		ConfigGroup group;
		group.p = new ConfigGroupPrivate;
		group.p->index = index;
		group.p->parent = p;
		group.p->config = p->config;

		Q_ASSERT_X(index >= 0, "ConfigGroup::at", "index out of range");
		if(ConfigEntry::Ptr entry = p->entries.at(0))
		{
			if((entry->type & ConfigEntry::AnyGroup) == ConfigEntry::Map)
			{
				entry->map.clear();
				entry->type &= ~ConfigEntry::Map;
			}
			entry->type |= ConfigEntry::Array;
			if(entry->array.size() <= index)
				entry->array.resize(index + 1);
			ConfigEntry::Ptr new_entry(new ConfigEntry);
			entry->array[index] = new_entry;
			group.p->entries << new_entry;
		}
		else
			return group;
		for(int i = 1; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Array))
			{
				ConfigEntry::Ptr entry = p->entries.at(i);
				if(index < 0 || index >= entry->array.size())
					continue;
				group.p = new ConfigGroupPrivate;
				group.p->name = QString::number(index);
				group.p->parent = p;
				group.p->config = p->config;
				group.p->entries << entry->array[index];
			}
		return group;
	}

	void ConfigGroup::removeAt(int index)
	{
		if(ConfigEntry::Ptr entry = p->entries.at(0))
			if((entry->type & ConfigEntry::Array) && index >= 0 && index < entry->array.size())
				entry->array.remove(index);
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
		QVariant result;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map))
			{
				ConfigEntry::Ptr entry = p->entries.at(i);
				ConfigEntry::EntryMap::const_iterator it = entry->map.constFind(key);
				if(it != entry->map.end() && ((*it)->type & ConfigEntry::Value))
				{
					result = (*it)->value;
					break;
				}
			}
		if(result.isNull())
			result = def;
		if((type & Config::Crypted) && !result.isNull())
			return CryptoService::decrypt(result.toByteArray());
		return result;
	}

	void ConfigGroup::writeEntry(const QString &key, const QVariant &value, Config::ValueFlag type)
	{
		ConfigGroup group = this->group(key);
		if(!group.isValid())
			return;
		if(ConfigEntry::Ptr entry = group.p->entries.at(0))
		{
			entry->type = ConfigEntry::Value;
			entry->value = (type & Config::Crypted) ? CryptoService::crypt(value) : value;
			entry->dirty = true;
		}
//		if(ConfigEntry::Ptr entry = group.p->entries.at(0))
//		{
//			entry->type = ConfigEntry::Map;
//			ConfigEntry::Ptr new_entry(new ConfigEntry);
//			new_entry->dirty = true;
//			new_entry->value = (type & Config::Crypted) ? CryptoService::crypt(value) : value;
//			new_entry->type = ConfigEntry::Value;
//			entry->map.insert(key, new_entry);
//		}
	}

	void ConfigGroup::sync()
	{
		config().sync();
	}
}
