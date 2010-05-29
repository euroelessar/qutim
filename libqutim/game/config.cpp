/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "config.h"
#include "../cryptoservice.h"
#include <QStringList>
#include <QFileInfo>
#include "../jsonfile.h"
#include "../systeminfo.h"

namespace qutim_sdk_0_3
{
	namespace Game
	{
		struct ConfigLevel
		{
			inline ConfigLevel() : deleteOnDestroy(false), typeMap(true), arrayElement(false), list(0) {}
			inline ~ConfigLevel();
			bool deleteOnDestroy;
			bool typeMap;
			bool arrayElement;
			union {
				QVariantList *list;
				QVariantMap *map;
			};
		};
		
		ConfigLevel::~ConfigLevel()
		{
			if (deleteOnDestroy && typeMap)
				delete map;
			else if (deleteOnDestroy && !typeMap)
				delete list;
		}

		class ConfigPrivate : public QSharedData
		{
		public:
			inline ConfigPrivate() { levels << new ConfigLevel(); }
			inline ~ConfigPrivate() { qDeleteAll(levels); }
			inline ConfigLevel *current() const { return levels.at(0); }
			QList<ConfigLevel *> levels;
			QString fileName;
		private:
			Q_DISABLE_COPY(ConfigPrivate)
		};
		
		Config::Config(const QVariantList &list) : d_ptr(new ConfigPrivate)
		{
			Q_D(Config);
			d->current()->deleteOnDestroy = true;
			d->current()->typeMap = false;
			d->current()->list = new QVariantList(list);
		}

		Config::Config(QVariantList *list) : d_ptr(new ConfigPrivate)
		{
			Q_D(Config);
			d->current()->typeMap = false;
			d->current()->list = list;
		}

		Config::Config(const QVariantMap &map) : d_ptr(new ConfigPrivate)
		{
			Q_D(Config);
			d->current()->deleteOnDestroy = true;
			d->current()->map = new QVariantMap(map);
		}

		Config::Config(QVariantMap *map) : d_ptr(new ConfigPrivate)
		{
			Q_D(Config);
			d->current()->map = map;
		}

		Config::Config(const QString &path) : d_ptr(new ConfigPrivate)
		{
			Q_D(Config);
			QFileInfo info(path);
			JsonFile file;
			if (info.isAbsolute())
				d->fileName = path;
			else
				d->fileName = SystemInfo::getDir(SystemInfo::ConfigDir).filePath(path);
			file.setFileName(d->fileName);
			QVariant var;
			file.load(var);
			d->current()->deleteOnDestroy = true;
			if (var.type() == QVariant::Map) {
				d->current()->map = new QVariantMap(var.toMap());
			} else if (var.type() == QVariant::List) {
				d->current()->typeMap = false;
				d->current()->list = new QVariantList(var.toList());
			} else {
				d->current()->map = new QVariantMap();
			}
		}
		
		Config::Config(const Config &other) : d_ptr(other.d_ptr)
		{
		}

		Config &Config::operator =(const Config &other)
		{
			d_ptr = other.d_ptr;
			return *this;
		}

		Config::~Config()
		{
			sync();
			qDeleteAll(d_func()->levels);
		}

		Config Config::group(const QString &name)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->typeMap);
			QVariant &var = (*(d->current()->map))[name];
			if (var.type() != QVariant::Map)
				var.setValue(QVariantMap());
			QVariantMap *map = reinterpret_cast<QVariantMap*>(var.data());
			return Config(map);
		}

		QStringList Config::childGroups() const
		{
			Q_D(const Config);
			if (!d->current()->typeMap)
				return QStringList();
			QStringList groups;
			QVariantMap::iterator it = d->current()->map->begin();
			for (; it != d->current()->map->end(); it++) {
				if (it.value().type() == QVariant::Map)
					groups << it.key();
			}
			return groups;
		}

		QStringList Config::childKeys() const
		{
			Q_D(const Config);
			if (!d->current()->typeMap)
				return QStringList();
			QStringList groups;
			QVariantMap::iterator it = d->current()->map->begin();
			for (; it != d->current()->map->end(); it++) {
				if (it.value().type() != QVariant::Map)
					groups << it.key();
			}
			return groups;
		}

		void Config::beginGroup(const QString &name)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->typeMap);
			ConfigLevel * const l = new ConfigLevel;
			l->deleteOnDestroy = false;
			QVariant &var = (*(d->current()->map))[name];
			if (var.type() != QVariant::Map)
				var.setValue(QVariantMap());
			l->map = reinterpret_cast<QVariantMap*>(var.data());
			d->levels.prepend(l);
		}

		void Config::endGroup()
		{
			Q_D(Config);
			Q_ASSERT(d->levels.size() > 1);
			Q_ASSERT(d->current()->typeMap);
			delete d->levels.takeFirst();
		}

		void Config::remove(const QString &name)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->typeMap);
			d->current()->map->remove(name);
		}

		Config Config::arrayElement(int index)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->arrayElement || !d->current()->typeMap);
			ConfigLevel *l = 0;
			if (d->current()->arrayElement) {
				Q_ASSERT(d->levels.size() > 1);
				l = d->levels.at(1);
			} else if (!d->current()->typeMap) {
				l = d->current();
			}
			QVariant &var = (*(d->current()->list))[index];
			if (var.type() != QVariant::Map)
				var.setValue(QVariantMap());
			QVariantMap *map = reinterpret_cast<QVariantMap*>(var.data());
			return Config(map);
		}

		int Config::beginArray(const QString &name)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->typeMap);
			ConfigLevel * const l = new ConfigLevel;
			l->deleteOnDestroy = false;
			l->typeMap = false;
			QVariant &var = (*(d->current()->map))[name];
			if (var.type() != QVariant::List)
				var.setValue(QVariantList());
			l->list = reinterpret_cast<QVariantList*>(var.data());
			d->levels.prepend(l);
		}

		void Config::endArray()
		{
			Q_D(Config);
			Q_ASSERT(d->current()->arrayElement || !d->current()->typeMap);
			if (d->current()->arrayElement) {
				Q_ASSERT(d->levels.size() > 2);
				delete d->levels.takeFirst();
				endArray();
			} else if (!d->current()->typeMap) {
				Q_ASSERT(d->levels.size() > 1);
				delete d->levels.takeFirst();
			}
		}

		void Config::setArrayIndex(int index)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->arrayElement || !d->current()->typeMap);
			if (!d->current()->typeMap) {
				ConfigLevel * const l = new ConfigLevel();
				l->typeMap = false;
				l->arrayElement = true;
				d->levels.prepend(l);
			}
			if (d->current()->arrayElement) {
				Q_ASSERT(d->levels.size() > 1);
				ConfigLevel * const l = d->levels.at(1);
				Q_ASSERT(!l->typeMap);
				Q_ASSERT(index >= 0 && index < l->list->size());
				while (l->list->size() <= index)
					l->list->append(QVariantMap());
				QVariant &var = (*(l->list))[index];
				if (var.type() != QVariant::Map)
					var.setValue(QVariantMap());
				d->current()->map = reinterpret_cast<QVariantMap*>(var.data());
			}
		}

		void Config::remove(int index)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->arrayElement || !d->current()->typeMap);
			if (d->current()->arrayElement) {
				Q_ASSERT(d->levels.size() > 1);
				delete d->levels.takeFirst();
			}
			Q_ASSERT(!d->current()->typeMap);
			Q_ASSERT(index >= 0 && index < d->current()->list->size());
			d->current()->list->removeAt(index);
		}

		QVariant Config::value(const QString &key, const QVariant &def, ValueFlags type) const
		{
			Q_D(const Config);
			Q_ASSERT(d->current()->typeMap);
			QVariant var = d->current()->map->value(key);
			if (type & Config::Crypted)
				return var.isNull() ? def : CryptoService::decrypt(var);
			else
				return var.isNull() ? def : var;
		}

		void Config::setValue(const QString &key, const QVariant &value, ValueFlags type)
		{
			Q_D(Config);
			Q_ASSERT(d->current()->typeMap);
			QVariant var = (type & Config::Crypted) ? CryptoService::crypt(value) : value;
			d->current()->map->insert(key, var);
		}

		void Config::sync()
		{
			Q_D(Config);
			if (!d->fileName.isEmpty()) {
				JsonFile file;
				file.setFileName(d->fileName);
				if (d->current()->typeMap)
					file.save(*d->current()->map);
				else
					file.save(*d->current()->list);
			}
		}

	}
}
