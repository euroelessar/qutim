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

#include "localizationmodule.h"
#include "localizationsettings.h"
#include <qutim/settingslayer.h>
#include <qutim/systeminfo.h>
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include <qutim/thememanager.h>
#include <QtCore/QLocale>
#include <QtCore/QCoreApplication>
#include <QtCore/QResource>
#include <QtCore/QLibraryInfo>

namespace Core
{
using namespace qutim_sdk_0_3;

LocalizationModule::LocalizationModule()
{
	SettingsItem *item = new GeneralSettingsItem<LocalizationSettings>(
							 Settings::General,
							 Icon("preferences-desktop-locale"),
							 QT_TRANSLATE_NOOP("Settings", "Localization"));
	item->connect(SIGNAL(saved()), this, SLOT(onSettingsSave()));
	Settings::registerItem(item);
	onSettingsSave();
}

QString getLocaleLanguage(const QLocale &locale)
{
	return locale.name().section('_', 0, 0);
}

QString getLocaleCountry(const QLocale &locale)
{
	return locale.name().section('_', 1, 1);
}

void LocalizationModule::onSettingsSave()
{
	QStringList langs = ThemeManager::list("languages");
	QString lang = Config().group("localization").value("lang", QString());
	if (langs.contains(lang) || lang == QLatin1String("en_EN")) {
		loadLanguage(QStringList() << lang);
	} else {
		loadLanguage(determineSystemLocale());
	}
}

QStringList LocalizationModule::determineSystemLocale()
{
	// TODO: Check if it works correct
	QStringList langs = ThemeManager::list("languages");
	QStringList langsByCountry;
	{
		QByteArray data = qgetenv("LANGUAGE");
		if (data.isEmpty())
			data = qgetenv("LANG");
		if (!data.isEmpty()) {
			// Environmental variables looks like "ru_RU.UTF-8:en_US.UTF-8"
			// for multiple languages, but we don't need encoding
			foreach (const QString &lang, QString(QLatin1String(data)).split(':')) {
				QString sublang = lang.section('.', 0, 0);
				if (langs.contains(sublang)
				        || (sublang.contains(QLatin1Char('_'))
				            && langs.contains(sublang.section(QLatin1Char('_'), 0, 0)))) {
					langsByCountry << sublang;
				}
			}
		}
	}
	if (langsByCountry.isEmpty()) {
		// Try to determine language by system locale
		QLocale locale = QLocale::system();
		// Don't use chezh locale for posix
		if (locale.language() == QLocale::C)
			return QStringList();
		QStringList langsByLang = langs.filter(getLocaleLanguage(locale),
											   Qt::CaseSensitive);
		// There may be different localizations for one language, but different countries
		langsByCountry = langsByLang.filter(getLocaleCountry(locale),
											Qt::CaseSensitive);
		// But if we have no certain match we should load all localizations
		if (langsByCountry.isEmpty())
			langsByCountry = langsByLang;
	}
	return langsByCountry;
}

Q_GLOBAL_STATIC(QList<QTranslator *>, translatorsCache)
Q_GLOBAL_STATIC(QStringList, authorsCache)

void LocalizationModule::loadLanguage(const QStringList &langs)
{
	QList<QTranslator *> &translators = *translatorsCache();
	foreach (QTranslator *translator, translators)
		qApp->removeTranslator(translator);
	foreach (const QString &path, *authorsCache())
		QResource::unregisterResource(path);
	authorsCache()->clear();
	qDeleteAll(translators);
	translators.clear();
	QStringList paths;
	foreach (const QString &lang, langs) {
		QString path = ThemeManager::path("languages", lang);
		if (path.isEmpty() && lang.contains(QLatin1Char('_')))
			path = ThemeManager::path("languages", lang.section(QLatin1Char('_'), 0, 0));
		if (!path.isEmpty())
			paths << path;
	}
	paths.removeDuplicates();
	if (paths.isEmpty()) {
		QLocale::setDefault(QLocale(QLocale::English));
		return;
	}
	// For jabber's xml:lang
	QLocale::setDefault(QLocale(langs.first()));
	// Firstly we should try to load Qt's translation for selected languages
	QString qtPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
	foreach (const QString &lang, langs) {
		QTranslator *translator = new QTranslator(qApp);
		if (!translator->load(QLatin1String("qt_") + lang, qtPath)) {
			delete translator;
		} else {
			qApp->installTranslator(translator);
			translators << translator;
		}
	}
	// And our local translations afterwords
	foreach (const QDir &dir, paths) {
		QStringList files = dir.entryList(QStringList() << "*.qm", QDir::Files);

		foreach (const QString &file, files) {
			QTranslator *translator = new QTranslator(qApp);
			if (!translator->load(file, dir.absolutePath())) {
				delete translator;
			} else {
				qApp->installTranslator(translator);
				translators << translator;
			}
		}

		files = dir.entryList(QStringList() << "*.rcc", QDir::Files);
		foreach (const QString &file, files) {
			if (QResource::registerResource(dir.filePath(file)))
				authorsCache()->append(file);
		}
	}
}
}

