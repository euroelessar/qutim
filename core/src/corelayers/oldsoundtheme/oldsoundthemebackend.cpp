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

#include "oldsoundthemebackend.h"
#include <QStringList>
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QStringBuilder>
#include <qutim/thememanager.h>

namespace Core
{
QStringList OldSoundThemeBackend::themeList()
{
	QStringList themes = ThemeManager::list("sounds");
	QStringList themeList;
	foreach (const QString &name, themes) {
		QDir dir(ThemeManager::path("sounds", name));
		QStringList entries = dir.entryList(QStringList("*.xml"), QDir::Files);
		if (entries.isEmpty())
			continue;
		bool single = entries.size() == 1;
		foreach (const QString &entry, entries) {
			QFile file(dir.filePath(entry));
			if (!file.open(QIODevice::ReadOnly))
				continue;
			QDomDocument doc;
			doc.setContent(&file);
			if (doc.doctype().name() == QLatin1String("qutimsounds")) {
				if (single) {
					themeList << name;
				} else {
					themeList << (name % " (" % entry.mid(0, entry.length() - 4) % ")");
				}
			}
		}
	}
	return themeList;
}

SoundThemeProvider *OldSoundThemeBackend::loadTheme(const QString &name)
{
	QString themeName = name;
	QString themeVariant;
	if (name.endsWith(")") && name.contains(" (")) {
		int index = name.indexOf(" (");
		themeName = name.mid(0, index);
		themeVariant = name.mid(index + 2, name.length() - 3 - index);
	}
	return new OldSoundThemeProvider(name, ThemeManager::path("sounds", themeName), themeVariant);
}
}

