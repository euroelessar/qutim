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
#include "src/modulemanagerimpl.h"
#include "localizationsettings.h"
#include "libqutim/settingslayer.h"
#include "libqutim/systeminfo.h"
#include "libqutim/icon.h"
#include "libqutim/configbase.h"
#include <QtCore/QLocale>
#include <QtCore/QCoreApplication>

namespace Core
{
	static CoreModuleHelper<LocalizationModule, StartupModule> acc_creator_static(
			QT_TRANSLATE_NOOP("Plugin", "Localization"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM localization engine")
			);

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
		QStringList langs = listThemes("languages");
		QString lang = Config().group("localization").value("lang", QString());
		if (langs.contains(lang) || lang == QLatin1String("en_EN")) {
			loadLanguage(lang);
		} else {
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
			QStringList langsByCountry = langs.filter(country, Qt::CaseSensitive);
			if (langsByCountry.isEmpty())
				langsByCountry = langs;
			loadLanguage(langsByCountry.first());
		}
	}

	void LocalizationModule::loadLanguage(const QString &lang)
	{
		foreach (QTranslator *translator, m_translators)
			qApp->removeTranslator(translator);
		qDeleteAll(m_translators);
		m_translators.clear();

		QString path = getThemePath("languages", lang);
		if (path.isEmpty()) {
			QLocale::setDefault(QLocale::c());
			return;
		}
		
		QDir dir = path;
		QStringList files = dir.entryList(QStringList() << "*.qm", QDir::Files);

		// For jabber's xml:lang
		QLocale::setDefault(QLocale(lang));

		foreach (const QString &file, files) {
			QTranslator *translator = new QTranslator(this);
			if (!translator->load(file, path)) {
				delete translator;
			} else {
				qApp->installTranslator(translator);
				m_translators << translator;
			}
		}
	}
}
