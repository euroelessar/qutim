/****************************************************************************
 *  serviceicons.cpp
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
		ConfigGroup config = Config().group("serviceicons");
		QString path = ThemeManager::path("protocolicons", config.value<QString>("theme", "default"));
		if (path.isEmpty()) {
			QStringList themes = ThemeManager::list("protocolicons");
			if (themes.isEmpty())
				return;
			path = ThemeManager::path("protocolicons", themes.first());
		}
		Config theme(path + "/Icons.plist");
		foreach (const QString &groupName, theme.childGroups()) {
			ConfigGroup group = theme.group(groupName);
			QStringList protocols = group.childKeys();
			foreach (const QString &protocol, protocols) {
				QString iconPath = group.value(protocol, QString());
				if (iconPath.isEmpty())
					continue;
				m_icons["im-"+protocol.toLower()].addPixmap(QPixmap(path % QLatin1Char('/') % iconPath));
			}
		}
	}

	ServiceIcons::~ServiceIcons()
	{
	}

	QIcon ServiceIcons::getIcon(const QString &name)
	{
		return m_icons.value(name, QIcon());
	}
}
