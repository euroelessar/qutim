/****************************************************************************
 *  configbase_p.h
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

#ifndef CONFIGBASE_P_H
#define CONFIGBASE_P_H

#include "configbase.h"
#include "configbackend.h"
#include <QDateTime>

namespace qutim_sdk_0_3
{
	typedef QList<ConfigEntry::WeakPtr>        ConfigEntryList;
	typedef QList<ConfigEntry::Ptr>            ConfigStrongEntryList;
//	typedef ConfigStrongEntryList              ConfigEntryList;
	typedef QPair<QByteArray, ConfigBackend *> ConfigBackendInfo;

	class ConfigBasePrivate : public QSharedData
	{
	public:
		inline ConfigBasePrivate() {}
		inline ConfigBasePrivate(const ConfigBasePrivate &other)
				: QSharedData(other), entries(other.entries) {}
		// First one must be real data, other fallbacks
		ConfigEntryList entries;
	};

	typedef QExplicitlySharedDataPointer<ConfigBasePrivate> ConfigBasePrivatePtr;

	class ConfigGroupPrivate : public ConfigBasePrivate
	{
	public:
		inline ConfigGroupPrivate() : index(-1) {}
		inline ConfigGroupPrivate(const ConfigGroupPrivate &other)
				: ConfigBasePrivate(other), name(other.name), index(other.index),
				parent(other.parent), config(other.config) {}
		QString name;
		int index;
		QExplicitlySharedDataPointer<ConfigGroupPrivate> parent;
		QExplicitlySharedDataPointer<ConfigPrivate> config;
//		// First one must be real data, other fallbacks
//		ConfigEntryList entries;
	};

	struct ConfigEntryInfo
	{
		ConfigEntry::Ptr root;
		QString file;
		ConfigBackend *backend;
		QDateTime last_change;
	};
	typedef QList<ConfigEntryInfo> ConfigEntryInfoList;

	class ConfigPrivate : public ConfigBasePrivate
	{
	public:
		inline ConfigPrivate() {}
		inline ConfigPrivate(const ConfigPrivate &other)
				: ConfigBasePrivate(other), file(other.file) {}
		QString file;
//		// First one must be real data, other fallbacks
//		ConfigStrongEntryList entries;
		ConfigEntryInfoList root_entries;
		LIBQUTIM_EXPORT static QList<ConfigBackendInfo> config_backends;
	};
}

#endif // CONFIGBASE_P_H
