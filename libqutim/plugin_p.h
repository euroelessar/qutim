#ifndef PLUGIN_P_H
#define PLUGIN_P_H

#include "plugin.h"

namespace qutim_sdk_0_3
{
	class PersonInfoData : public QSharedData
	{
	public:
		PersonInfoData();
		PersonInfoData(const PersonInfoData &other);
		LocalizedString name;
		LocalizedString task;
		QString email;
		QString web;
	};

	class PluginInfoData : public QSharedData
	{
	public:
		PluginInfoData();
		PluginInfoData(const PluginInfoData &other);
		QList<PersonInfo> authors;
		LocalizedString name;
		LocalizedString description;
		quint32 version;
		ExtensionIcon icon;
	};

	class ExtensionInfoData : public QSharedData
	{
	public:
		ExtensionInfoData();
		ExtensionInfoData(const ExtensionInfoData &other);
		LocalizedString name;
		LocalizedString description;
		const ObjectGenerator *gen;
		ExtensionIcon icon;
		PluginInfo plugin;
	};

	struct PluginPrivate
	{
		PluginPrivate() : is_inited(false) {}
		PluginInfo info;
		QList<ExtensionInfo> extensions;
		bool is_inited;
		// TODO: make method real
		bool validate();
	};
}

#endif // PLUGIN_P_H
