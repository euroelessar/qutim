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

#include "localizationsettings.h"
#include "ui_localizationsettings.h"
#include <qutim/configbase.h>
#include <QStringBuilder>
#include <QDebug>
#include <QDir>
#include <QTranslator>
#include <qutim/icon.h>
#include <qutim/thememanager.h>

using namespace qutim_sdk_0_3;

namespace Core
{

LocalizationSettings::LocalizationSettings() :
	m_ui(new Ui::LocalizationSettings)
{
	m_ui->setupUi(this);
	QStringList langs = ThemeManager::list("languages");
	Icon icon("preferences-desktop-locale");
	if (!langs.contains(QLatin1String("en_EN"))) {
		QListWidgetItem *item = new QListWidgetItem(icon, QLatin1String("English"),
													m_ui->languagesList);
		m_items.insert(QLatin1String("en_EN"), item);
		item->setData(Qt::UserRole, QLatin1String("en_EN"));
	}
	foreach (const QString &lang, langs) {
		QString text;
		QDir dir = ThemeManager::path("languages", lang);
		if (dir.exists("core.qm")) {
			QTranslator translator;
			translator.load(QLatin1String("core.qm"), dir.absolutePath());
			QString language = translator.translate("Localization", "<Language>", "Localized language name");
			if (!language.isEmpty()) {
				text = language;
				QString country = translator.translate("Localization", "<Country>",
													   "Localized country name, empty if localization is country-independent");
				if (!country.isEmpty()) {
					text += QLatin1String(" (");
					text += country;
					text += QLatin1String(")");
				}
			}
		}
		if (text.isEmpty()) {
			QLocale locale(lang);
			text = QLocale::languageToString(locale.language());
			if (locale.country() != QLocale::AnyCountry) {
				text += QString(QLatin1Literal(" (")
								% QLocale::countryToString(locale.country())
								% QLatin1Literal(")"));
			}
		}
		QListWidgetItem *item = new QListWidgetItem(icon, text, m_ui->languagesList);
		m_items.insert(lang, item);
		item->setData(Qt::UserRole, lang);
	}
	m_ui->languagesList->sortItems();
	listenChildrenStates();
}

LocalizationSettings::~LocalizationSettings()
{
	delete m_ui;
}

void LocalizationSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("localization");
	QString lang = cfg.value("lang", QString());
	m_ui->languagesList->setCurrentItem(m_items.value(lang, 0));
}

void LocalizationSettings::saveImpl()
{
	if (QListWidgetItem *item = m_ui->languagesList->currentItem()) {
		Config cfg;
		cfg.beginGroup("localization");
		cfg.setValue("lang", item->data(Qt::UserRole));
	}
}

void LocalizationSettings::cancelImpl()
{
	loadImpl();
}

void LocalizationSettings::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
}

