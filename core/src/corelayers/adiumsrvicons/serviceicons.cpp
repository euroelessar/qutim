/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "serviceicons.h"
#include <qutim/configbase.h>
#include <qutim/thememanager.h>
#include <qutim/servicemanager.h>
#include <QStringBuilder>

using namespace qutim_sdk_0_3;

namespace Adium
{
	ServiceIcons::ServiceIcons()
	{
		Config config;
		config.beginGroup("serviceicons");
		QString path = ThemeManager::path("protocolicons", config.value("theme", "default"));
		if (path.isEmpty()) {
			QStringList themes = ThemeManager::list("protocolicons");
			if (themes.isEmpty())
				return;
			path = ThemeManager::path("protocolicons", themes.first());
		}
		config = Config(path + "/Icons.plist");
		foreach (const QString &groupName, config.childGroups()) {
			config.beginGroup(groupName);
			QStringList protocols = config.childKeys();
			foreach (const QString &protocol, protocols) {
				QString iconPath = config.value(protocol, QString());
				if (iconPath.isEmpty())
					continue;
				m_icons["im-"+protocol.toLower()].addFile(path % QLatin1Char('/') % iconPath);
			}
			config.endGroup();
		}
	}

	ServiceIcons::~ServiceIcons()
	{
	}
	
	QIcon ServiceIcons::doLoadIcon(const QString &name)
	{
		return m_icons.value(name, QIcon());
	}
	
	QMovie *ServiceIcons::doLoadMovie(const QString &)
	{
		return 0;
	}
	
	QString ServiceIcons::doIconPath(const QString &, uint)
	{
		return QString();
	}
	
	QString ServiceIcons::doMoviePath(const QString &, uint)
	{
		return QString();
	}
}

