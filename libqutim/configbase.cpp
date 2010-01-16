/****************************************************************************
 *  configbase.cpp
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

#include "configbase_p.h"
#include "cryptoservice.h"
#include "systeminfo.h"
#include <QStringList>
#include <QSet>
#include <QFileInfo>
#include <QStringBuilder>
#include <QCache>

namespace qutim_sdk_0_3
{
	QList<ConfigBackendInfo> ConfigPrivate::config_backends = QList<ConfigBackendInfo>();

	static QCache<QString, ConfigEntryInfo> *cached_entries = 0;

	static const ConfigEntryInfo &parseConfigEntry(const QString &filepath, ConfigBackend *backend)
	{
		if(!cached_entries)
		{
			cached_entries = new QCache<QString, ConfigEntryInfo>();
			cached_entries->setMaxCost(10); // But it's strange...
		}
		QFileInfo info(filepath);
		QString id = backend->backendName() % QLatin1Char('\0')
//					 % QString::number(info.lastModified().toTime_t())
//					 % QLatin1Char('\0')
					 % filepath;
		ConfigEntryInfo *entry_info = cached_entries->object(id);
		if(entry_info && entry_info->last_change >= info.lastModified())
			return *entry_info;
		entry_info = new ConfigEntryInfo;
		entry_info->root = backend->parse(filepath);
		entry_info->file = filepath;
		entry_info->backend = backend;
		entry_info->last_change = info.lastModified();
		if(entry_info->root.isNull())
			entry_info->root = ConfigEntry::Ptr(new ConfigEntry());
		cached_entries->insert(id, entry_info);
		return *entry_info;
	}

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
		for (int i = 0; i < p->entries.size(); i++) {
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map))
			{
				ConfigEntry::Ptr entry = p->entries.at(i);
				ConfigEntry::EntryMap::const_iterator it = entry->map.find(name);
				if(it != entry->map.constEnd())
					group.p->entries.append(*it);
			}
		}
		if (const ConfigGroup *me = dynamic_cast<const ConfigGroup *>(this)) {
			group.p->parent = me->p;
			group.p->config = me->p->config;
		} else {
			const Config *meConfig = static_cast<const Config *>(this);
			group.p->parent.reset();
			group.p->config = meConfig->p;
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
				if(entry->type & ConfigEntry::Array)
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
		for (int i = 1; i < p->entries.size(); i++) {
			if (p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map)) {
				ConfigEntry::Ptr entry = p->entries.at(i);
				ConfigEntry::EntryMap::const_iterator it = entry->map.constFind(name);
				if (it != entry->map.end())
					group.p->entries.append(*it);
			}
		}
		if (const ConfigGroup *me = dynamic_cast<const ConfigGroup *>(this)) {
			group.p->parent = me->p;
			group.p->config = me->p->config;
		} else {
			const Config *meConfig = static_cast<const Config *>(this);
			group.p->parent.reset();
			group.p->config = meConfig->p;
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

	inline ConfigBackend *choose_backend(const QList<ConfigBackendInfo> &backends, const QString &backend = QString())
	{
		for(int i = 0; i < backends.size(); i++)
			if(backends.at(i).first == backend)
				return backends.at(i).second;
		return backends.at(0).second;
	}

	Config::Config(const QString &filename, OpenFlags flags, const QString &backend)
	{
		Q_UNUSED(flags);
		if(ConfigPrivate::config_backends.isEmpty())
			return;
		QFileInfo info(filename);
		ConfigBackend *backend_ptr = 0;
		QString filepath;
		if(info.isAbsolute())
		{
			backend_ptr = choose_backend(ConfigPrivate::config_backends,
														backend.isEmpty() ? info.suffix() : backend);
			filepath = filename;
		}
		else if(filename.isEmpty())
		{
			backend_ptr = ConfigPrivate::config_backends.at(0).second;
			filepath = SystemInfo::getPath(SystemInfo::ConfigDir)
					   % QLatin1Char('/')
					   % QLatin1Literal("profile.")
					   % QLatin1String(ConfigPrivate::config_backends.at(0).first.constData());
		}
		else
		{
			backend_ptr = ConfigPrivate::config_backends.at(0).second;
			filepath = SystemInfo::getPath(SystemInfo::ConfigDir)
					   % QLatin1Char('/')
					   % filename.toLower()
					   % QLatin1Char('.')
					   % QLatin1String(ConfigPrivate::config_backends.at(0).first.constData());
		}
		const ConfigEntryInfo &entry_info = parseConfigEntry(filepath, backend_ptr);
//		ConfigEntry::Ptr entry = backend_ptr->parse(filepath);
//		if(entry.isNull())
//			entry = ConfigEntry::Ptr(new ConfigEntry());
		p = new ConfigPrivate;
		p->file = filename;
//		ConfigEntryInfo entry_info = { entry, filepath, backend_ptr };
		p->root_entries << entry_info;
		p->entries << entry_info.root.toWeakRef();
	}

	Config::Config(const QStringList &files_, OpenFlags flags, const QString &backend)
	{
		if(ConfigPrivate::config_backends.isEmpty())
			return;
		QStringList files = files_;
		QSet<QString> check;
		bool first = true;
		for(int i = 0; i < files.size(); i++)
		{
			QString filename = files.at(i);
			QFileInfo info(filename);
			ConfigBackend *backend_ptr = 0;
			QString filepath;
			if(info.isAbsolute())
			{
				backend_ptr = choose_backend(ConfigPrivate::config_backends,
															backend.isEmpty() ? info.suffix() : backend);
				filepath = filename;
			}
			else if(filename.isEmpty())
			{
				backend_ptr = ConfigPrivate::config_backends.at(0).second;
				QString postfix = QLatin1Char('/')
								  % QLatin1Literal("profile.")
								  % QLatin1String(ConfigPrivate::config_backends.at(0).first.constData());
				filepath = SystemInfo::getPath(SystemInfo::ConfigDir) + postfix;
				if(flags & IncludeGlobals)
					files += SystemInfo::getPath(SystemInfo::SystemConfigDir) + postfix;
			}
			else
			{
				backend_ptr = ConfigPrivate::config_backends.at(0).second;
				QString postfix = QLatin1Char('/')
								  % filename.toLower()
								  % QLatin1Char('.')
								  % QLatin1String(ConfigPrivate::config_backends.at(0).first.constData());
				filepath = SystemInfo::getPath(SystemInfo::ConfigDir) + postfix;
				if(flags & IncludeGlobals)
					files += SystemInfo::getPath(SystemInfo::SystemConfigDir) + postfix;
			}
			if(check.contains(filepath))
				continue;
			else
				check |= filepath;
			const ConfigEntryInfo &entry_info = parseConfigEntry(filepath, backend_ptr);
//			ConfigEntry::Ptr entry = backend_ptr->parse(filepath);
			if(first)
			{
//				if(entry.isNull())
//					entry = ConfigEntry::Ptr(new ConfigEntry());
				p = new ConfigPrivate;
				p->file = filename;
				first = false;
			}
//			if(entry.isNull())
//				continue;
//			ConfigEntryInfo entry_info = { entry, filepath, backend_ptr };
			p->root_entries << entry_info;
			p->entries << entry_info.root.toWeakRef();
		}
	}
		
	Config::Config(const Config &other) : p(other.p)
	{
	}

	Config::Config(const QExplicitlySharedDataPointer<ConfigPrivate> &other) : p(other)
	{
	}
		
	Config &Config::operator =(const Config &other)
	{
		p = other.p;
		return *this;
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
		
	ConfigGroup &ConfigGroup::operator =(const ConfigGroup &other)
	{
		p = other.p;
		return *this;
	}

	QString ConfigGroup::name() const
	{
		return isValid() ? p->name : QString();
	}

	bool ConfigBase::isValid() const
	{
		ConfigBasePrivatePtr p = get_p();
		return p && !p->entries.isEmpty();
	}

	bool ConfigGroup::isMap() const
	{
		if(!isValid())
			return false;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map))
				return true;
		return false;
	}

	bool ConfigGroup::isArray() const
	{
		if(!isValid())
			return false;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Array))
				return true;
		return false;
	}

	bool ConfigGroup::isValue() const
	{
		if(!isValid())
			return false;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Value))
				return true;
		return false;
	}

	int ConfigGroup::arraySize() const
	{
		if(!isValid())
			return 0;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Array))
				return p->entries.at(i).toStrongRef()->array.size();
		return 0;
	}

	const ConfigGroup ConfigGroup::at(int index) const
	{
		ConfigGroup group;
		if(!isValid())
			return group;
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
		if(!isValid())
			return group;
		group.p = new ConfigGroupPrivate;
		group.p->index = index;
		group.p->parent = p;
		group.p->config = p->config;

		Q_ASSERT_X(index >= 0, "ConfigGroup::at", "index out of range");
		if(ConfigEntry::Ptr entry = p->entries.at(0))
		{
			if(entry->type & ConfigEntry::Map)
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
		if(!isValid())
			return;
		if(ConfigEntry::Ptr entry = p->entries.at(0))
			if((entry->type & ConfigEntry::Array) && index >= 0 && index < entry->array.size())
				entry->array.remove(index);
	}

	ConfigGroup ConfigGroup::parent()
	{
		ConfigGroup group;
		if(!isValid())
			return group;
		group.p = p->parent;
		return group;
	}

	const ConfigGroup ConfigGroup::parent() const
	{
		ConfigGroup group;
		if(!isValid())
			return group;
		group.p = p->parent;
		return group;
	}

	Config ConfigGroup::config()
	{
		Config config;
		if(!isValid())
			return config;
		config.p = p->config;
		return config;
	}

	const Config ConfigGroup::config() const
	{
		Config config;
		if(!isValid())
			return config;
		config.p = p->config;
		return config;
	}

	QVariant ConfigBase::value(const QString &key, const QVariant &def, Config::ValueFlags type) const
	{
		ConfigBasePrivatePtr p = get_p();
		QVariant result;
		if(!isValid())
			return result;
		for(int i = 0; i < p->entries.size(); i++)
			if(p->entries.at(i) && (p->entries.at(i).toStrongRef()->type & ConfigEntry::Map))
			{
				ConfigEntry::Ptr entry = p->entries.at(i);
				ConfigEntry::EntryMap::const_iterator it = entry->map.constFind(key);
				if((it != entry->map.constEnd()) && (*it) && ((*it)->type & ConfigEntry::Value))
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

	void ConfigBase::setValue(const QString &key, const QVariant &value, Config::ValueFlags type)
	{
		ConfigBasePrivatePtr p = get_p();
		if(!isValid())
			return;
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
		if(!isValid())
			return;
		config().sync();
	}
}
