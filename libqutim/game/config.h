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
			void beginGroup(const QString &name);
			void endGroup();
			void remove(const QString &name);
			
			Config arrayElement(int index);
			int beginArray(const QString &name);
			void endArray();
			int arraySize() const;
			void setArrayIndex(int index);
			void remove(int index);
			
			QVariant value(const QString &key, const QVariant &def = QVariant(), ValueFlags type = Normal) const;
			void setValue(const QString &key, const QVariant &value, ValueFlags type = Normal);
			
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
	}
}

#endif // CONFIG_H
