/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef PLUGIN_P_H
#define PLUGIN_P_H

#include "plugin.h"
#include <QVariant>

namespace qutim_sdk_0_3
{
	class PersonInfoData : public QSharedData
	{
	public:
		typedef QSharedDataPointer<PersonInfoData> Ptr;
		PersonInfoData(const QString &ocs = QString());
		PersonInfoData(const PersonInfoData &other);
		QVariantMap data() const;
		QString ocsUsername;
		mutable LocalizedString name;
		mutable LocalizedString task;
		mutable QString email;
		mutable QString web;
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
		int inited : 1;
		int loaded : 1;
		PluginInfo::Capabilities capabilities;
		ExtensionIcon icon;
		QString fileName;
        QString libraryName;
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

	class PluginPrivate
	{
	public:
		PluginPrivate() {}
		PluginInfo info;
		QList<ExtensionInfo> extensions;
		// TODO: make method real
		bool validate();
	};
}

#endif // PLUGIN_P_H

