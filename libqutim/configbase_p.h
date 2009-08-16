#ifndef CONFIGBASE_P_H
#define CONFIGBASE_P_H

#include "configbase.h"
#include "configbackend.h"

namespace qutim_sdk_0_3
{
	//typedef QList<ConfigEntry::WeakPtr>        ConfigEntryList;
	typedef QList<ConfigEntry::Ptr>            ConfigStrongEntryList;
	typedef ConfigStrongEntryList              ConfigEntryList;
	typedef QPair<QByteArray, ConfigBackend *> ConfigBackendInfo;

	class ConfigGroupPrivate : public QSharedData
	{
	public:
		inline ConfigGroupPrivate() : index(-1) {}
		inline ConfigGroupPrivate(const ConfigGroupPrivate &other)
				: QSharedData(other), name(other.name), index(other.index),
				parent(other.parent), config(other.config), entries(other.entries) {}
		QString name;
		int index;
		QExplicitlySharedDataPointer<ConfigGroupPrivate> parent;
		QExplicitlySharedDataPointer<ConfigPrivate> config;
		// First one must be real data, other fallbacks
		ConfigEntryList entries;
	};

	class ConfigPrivate : public QSharedData
	{
	public:
		inline ConfigPrivate() {}
		inline ConfigPrivate(const ConfigPrivate &other)
				: QSharedData(other), file(other.file), entries(other.entries) {}
		QString file;
		// First one must be real data, other fallbacks
		ConfigStrongEntryList entries;
		static QList<ConfigBackendInfo> config_backends;
	};
}

#endif // CONFIGBASE_P_H
