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

#include "thememanagerwrapper.h"
#include <qutim/thememanager.h>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;
ThemeManagerWrapper::ThemeManagerWrapper()
{

}

void ThemeManagerWrapper::init()
{
	qmlRegisterType<ThemeManagerWrapper>("org.qutim", 0, 3, "ThemeManager");
}

QString ThemeManagerWrapper::path(const QString& category, const QString &themeName)
{
	return ThemeManager::path(category,themeName);
}


QStringList ThemeManagerWrapper::list(const QString &category)
{
	return ThemeManager::list(category);
}

void ThemeManagerWrapper::addPath(const QString &path_, const QString &category)
{
	return ThemeManager::addPath(path_,category);
}

QList<QDir> ThemeManagerWrapper::categoryDirs(const QString &category)
{
	return ThemeManager::categoryDirs(category);
}

}

