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
#include "thememanager.h"
#include "systeminfo.h"
#include <QMultiMap>
#include <QDebug>

namespace qutim_sdk_0_3
{
class ThemeManagerData
{
public:
	ThemeManagerData() { paths << QDir(QLatin1String(":/")); }
	QList<QDir> paths;
	QMultiMap<QString,QDir> categoryPaths;
};

Q_GLOBAL_STATIC(ThemeManagerData, data)

ThemeManager::ThemeManager()
{
}

ThemeManager::~ThemeManager()
{
}

static inline QString getThemePath(QDir shareDir, const QString &themeName , const QString &category)
{
	if (shareDir.cd(category) && shareDir.cd(themeName))
		return shareDir.absolutePath();
	else
		return QString();
}

QString ThemeManager::path(const QString& category, const QString &themeName)
{
	QString themePath = getThemePath(SystemInfo::getDir(SystemInfo::ShareDir),themeName,category);
	if (themePath.isEmpty()) {
		themePath = getThemePath(SystemInfo::getDir(SystemInfo::SystemShareDir),themeName,category);
		if (themePath.isEmpty())
			themePath = getThemePath(QDir(":/"),themeName,category);
	}
	return themePath;
}

static inline QStringList listThemes(QDir shareDir, const QString &category)
{
	if (shareDir.cd(category))
		return shareDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	else
		return QStringList();
}

static QStringList convertToPaths(const QList<QDir> &dirs)
{
	QStringList paths;
	for (const QDir &dir : dirs)
		paths << dir.absolutePath();
	return paths;
}

QStringList ThemeManager::list(const QString &category)
{
	QStringList theme_list;
	theme_list << listThemes(SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::ShareDir),category);
	theme_list << listThemes(SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::SystemShareDir),category);
	foreach (const QDir &dir, data()->paths)
		theme_list << listThemes(dir, category);
	foreach (const QDir &dir, data()->categoryPaths.values(category))
		theme_list << listThemes(dir, category);
	theme_list.removeDuplicates();
	qDebug() << "ThemeManager::list, category:" << category
			 << "themes:" << theme_list
			 << "dirs:" << convertToPaths(QList<QDir>() << SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::ShareDir)
							<< SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::SystemShareDir)
							<< data()->paths
							<< data()->categoryPaths.values(category));
	return theme_list;
}

void ThemeManager::addPath(const QString &path_, const QString &category)
{
	Q_UNUSED(path_);
	Q_UNUSED(category);
}

QList<QDir> ThemeManager::categoryDirs(const QString &category)
{
	QList<QDir> list;
	QDir dir = SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::ShareDir);
	if (dir.cd(category))
		list << dir;

	dir = SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::SystemShareDir);
	if (dir.cd(category))
		list << dir;

	dir = QDir(":/" + category);
	if (dir.exists())
		list << dir;

	foreach (QDir dir, data()->paths) {
		if (dir.cd(category))
			list << dir;
	}

	foreach (QDir dir, data()->categoryPaths) {
		if (dir.cd(category))
			list << dir;
	}

	return list;
}
}

