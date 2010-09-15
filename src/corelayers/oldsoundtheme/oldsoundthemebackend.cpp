/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
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
