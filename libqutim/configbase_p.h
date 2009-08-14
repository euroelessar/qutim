#ifndef CONFIGBASE_P_H
#define CONFIGBASE_P_H

#include "configbase.h"
#include "configbackend.h"

namespace qutim_sdk_0_3
{
	typedef QList<ConfigEntry::WeakPtr> ConfigEntryList;

	class ConfigGroupPrivate : public QSharedData
	{
	public:
		inline ConfigGroupPrivate() : index(-1) {}
		QString name;
		int index;
		QExplicitlySharedDataPointer<ConfigGroupPrivate> parent;
		QExplicitlySharedDataPointer<ConfigPrivate> config;
		// First one must be real data, other fallbacks
		ConfigEntryList entry;
	};

	class ConfigPrivate : public QSharedData
	{
	public:
		QString file;
		ConfigEntryList entry;
	};
}

#endif // CONFIGBASE_P_H
