/****************************************************************************
 *  localizationmodule.cpp
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

#include "localizationmodule.h"
#include "localizationsettings.h"
#include <qutim/settingslayer.h>
#include <qutim/systeminfo.h>
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include <qutim/thememanager.h>
#include <QtCore/QLocale>
#include <QtCore/QCoreApplication>

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

	void LocalizationModule::onSettingsSave()
	{
		// TODO: Check if it works correct
		QStringList langs = ThemeManager::list("languages");
		QString lang = Config().group("localization").value("lang", QString());
		if (langs.contains(lang) || lang == QLatin1String("en_EN")) {
			loadLanguage(QStringList() << lang);
		} else {
			QStringList langsByCountry;
			{
				QByteArray data = qgetenv("LANGUAGE");
				if (data.isEmpty())
					data = qgetenv("LANG");
				if (!data.isEmpty())
					langsByCountry = QString(QLatin1String(data)).split(':');
			}
			if (langsByCountry.isEmpty()) {
				QLocale locale;
				if (!lang.isEmpty())
					locale = QLocale(lang);
				QStringList langsByLang = langs.filter(QLocale::languageToString(locale.language()),
													   Qt::CaseSensitive);
				if (langsByLang.isEmpty()) {
					locale = QLocale::system();
					langsByLang = langs.filter(QLocale::languageToString(locale.language()),
											   Qt::CaseSensitive);
					if (langsByLang.isEmpty())
						return;
				}
	
				QString country = QLocale::countryToString(locale.country());
				langsByCountry = langs.filter(country, Qt::CaseSensitive);
				if (langsByCountry.isEmpty())
					langsByCountry = langs;
			}
			loadLanguage(langsByCountry);
		}
	}

	void LocalizationModule::loadLanguage(const QStringList &langs)
	{
		foreach (QTranslator *translator, m_translators)
			qApp->removeTranslator(translator);
		qDeleteAll(m_translators);
		m_translators.clear();
		QStringList paths;
		foreach (const QString &lang, langs) {
			QString path = ThemeManager::path("languages", lang);
			if (!path.isEmpty())
				paths << path;
		}
		paths.removeDuplicates();
		if (paths.isEmpty()) {
			QLocale::setDefault(QLocale::c());
			return;
		}
		// For jabber's xml:lang
		QLocale::setDefault(QLocale(langs.first()));
		foreach (const QDir &dir, paths) {
			QStringList files = dir.entryList(QStringList() << "*.qm", QDir::Files);
	
			foreach (const QString &file, files) {
				QTranslator *translator = new QTranslator(this);
				if (!translator->load(file, dir.absolutePath())) {
					delete translator;
				} else {
					qApp->installTranslator(translator);
					m_translators << translator;
				}
			}
		}
	}
}
