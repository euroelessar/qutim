/****************************************************************************
 *  plugin.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

	ExtensionInfoData::ExtensionInfoData() : name(0), description(0), gen(0)
	{
	}

	ExtensionInfoData::ExtensionInfoData(const ExtensionInfoData &other)
			: QSharedData(other), name(other.name), description(other.description),
			gen(other.gen), icon(other.icon)
	{
	}

	ExtensionInfo::ExtensionInfo(const char *name, const char *description, const ObjectGenerator *generator, QIcon icon)
	{
		d = new ExtensionInfoData;
		setName(name);
		setDescription(description);
		setGenerator(generator);
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

	void Plugin::setInfo(const char *name, const char *description, quint32 version, QIcon icon)
	{
		m_info.setName(name);
		m_info.setDescription(description);
		m_info.setVersion(version);
		m_info.setIcon(icon);
	}

	void Plugin::addExtension(const char *name, const char *description, const ObjectGenerator *generator, QIcon icon)
	{
		Q_ASSERT_X(generator->metaObject(), "Plugin::addExtension", "ObjectGenerator must contain QMetaObject");
		m_extensions << ExtensionInfo(name, description, generator, icon);
	}
}
