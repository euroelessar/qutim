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

#ifndef CONFIG_H
#define CONFIG_H

#include "../libqutim_global.h"
#include <QVariant>
#include <QSharedData>

namespace qutim_sdk_0_3
{
	namespace Game
	{
		class ConfigPrivate;
		class ConfigBackendPrivate;
		
		class LIBQUTIM_EXPORT Config
		{
			Q_DECLARE_PRIVATE(Config)
		public:
			enum ValueFlag { Normal = 0x00, Crypted = 0x01 };
			Q_DECLARE_FLAGS(ValueFlags, ValueFlag)
			
			Config();
			Config(const QVariantList &list);
			Config(QVariantList *list);
			Config(const QVariantMap &map);
			Config(QVariantMap *map);
			Config(const QString &path);
			Config(const Config &other);
			Config &operator =(const Config &other);
			virtual ~Config();
			
			Config group(const QString &name);
			QStringList childGroups() const;
			QStringList childKeys() const;
			bool hasChildGroup(const QString &name) const;
			bool hasChildKey(const QString &name) const;
			void beginGroup(const QString &name);
			void endGroup();
			void remove(const QString &name);
			
			Config arrayElement(int index);
			int beginArray(const QString &name);
			void endArray();
			int arraySize() const;
			void setArrayIndex(int index);
			void remove(int index);
			
			template<typename T>
			T value(const QString &key, const T &def = T(), ValueFlags type = Normal) const;
			QVariant value(const QString &key, const QVariant &def = QVariant(), ValueFlags type = Normal) const;
			inline QString value(const QString &key, const QLatin1String &def, ValueFlags type = Normal) const;
			inline QString value(const QString &key, const char *def, ValueFlags type = Normal) const;
			template<typename T>
			void setValue(const QString &key, const T &value, ValueFlags type = Normal);
			void setValue(const QString &key, const QVariant &value, ValueFlags type = Normal);
			inline void setValue(const QString &key, const QLatin1String &value, ValueFlags type = Normal);
			inline void setValue(const QString &key, const char *value, ValueFlags type = Normal);
			
			void sync();
		private:
			QExplicitlySharedDataPointer<ConfigPrivate> d_ptr;
		};
	
		class LIBQUTIM_EXPORT ConfigBackend : public QObject
		{
			Q_OBJECT
			Q_DECLARE_PRIVATE(ConfigBackend)
		public:
			ConfigBackend();
			virtual ~ConfigBackend();
			
			virtual QVariant load(const QString &file) = 0;
			virtual void save(const QString &file, const QVariant &entry) = 0;
			
			QByteArray name() const;
		protected:
			virtual void virtual_hook(int id, void *data);
		private:
			QScopedPointer<ConfigBackendPrivate> d_ptr;
		};
		
		template<typename T>
		Q_INLINE_TEMPLATE T Config::value(const QString &key, const T &def, Config::ValueFlags type) const
		{
			return value(key, qVariantFromValue(def), type).value<T>();
		}
		
		template<typename T>
		Q_INLINE_TEMPLATE void Config::setValue(const QString &key, const T &value, Config::ValueFlags type)
		{
			setValue(key, qVariantFromValue(value), type);
		}
		
		QString Config::value(const QString &key, const QLatin1String &def, ValueFlags type) const
		{
			return value(key, QString(def), type);
		}
		
		QString Config::value(const QString &key, const char *def, ValueFlags type) const
		{
			return value(key, QString::fromUtf8(def), type);
		}
		
		void Config::setValue(const QString &key, const QLatin1String &value, ValueFlags type)
		{
			setValue(key, QString(value), type);
		}

		void Config::setValue(const QString &key, const char *value, ValueFlags type)
		{
			setValue(key, QString::fromUtf8(value), type);
		}
	}
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Game::Config)

#endif // CONFIG_H
