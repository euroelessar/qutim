#include "plugin.h"
#include <QtCore/QCoreApplication>

namespace qutim_sdk_0_3
{
	PersonInfoData::PersonInfoData() : name(0), task(0)
	{
	}

	PersonInfoData::PersonInfoData(const PersonInfoData &other)
			: QSharedData(other), name(other.name), task(other.task), email(other.email), web(other.web)
	{
	}

	PersonInfo::PersonInfo(const char *name, const char *task, const QString &email, const QString &web)
	{
		d = new PersonInfoData;
		setName(name);
		setTask(task);
		setEmail(email);
		setWeb(web);
	}

	QString PersonInfo::name() const
	{
		return qApp->translate("Author", d->name);
	}

	QString PersonInfo::task() const
	{
		return qApp->translate("Task", d->task);
	}

	PluginInfoData::PluginInfoData() : name(0), description(0)
	{
	}

	PluginInfoData::PluginInfoData(const PluginInfoData &other)
			: QSharedData(other), authors(other.authors), name(other.name),
			description(other.description), version(other.version), icon(other.icon)
	{
	}

	PluginInfo::PluginInfo(const char *name, const char *description, quint32 version, QIcon icon)
	{
		d = new PluginInfoData;
		setName(name);
		setDescription(description);
		setVersion(version);
		setIcon(icon);
	}

	PluginInfo &PluginInfo::addAuthor(const PersonInfo &author)
	{
		d->authors.append(author);
		return *this;
	}

	PluginInfo &PluginInfo::addAuthor(const char *name, const char *task, const QString &email, const QString &web)
	{
		d->authors.append(PersonInfo(name, task, email, web));
		return *this;
	}

	QString PluginInfo::name() const
	{
		return qApp->translate("Plugin", d->name);
	}

	QString PluginInfo::description() const
	{
		return qApp->translate("Plugin", d->description);
	}

	ExtensionInfoData::ExtensionInfoData() : name(0), description(0), meta(0)
	{
	}

	ExtensionInfoData::ExtensionInfoData(const ExtensionInfoData &other)
			: QSharedData(other), name(other.name), description(other.description),
			meta(other.meta), icon(other.icon)
	{
	}

	ExtensionInfo::ExtensionInfo(const char *name, const char *description, const QMetaObject *meta, QIcon icon)
	{
		d = new ExtensionInfoData;
		setName(name);
		setDescription(description);
		setMeta(meta);
		setIcon(icon);
	}

	QString ExtensionInfo::name() const
	{
		return qApp->translate("Plugin", d->name);
	}

	QString ExtensionInfo::description() const
	{
		return qApp->translate("Plugin", d->description);
	}

	Plugin::Plugin()
	{
	}
}
