/****************************************************************************
 *  localizationsettings.cpp
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

#include "localizationsettings.h"
#include "ui_localizationsettings.h"
#include "libqutim/configbase.h"
#include <QStringBuilder>
#include <QDebug>

using namespace qutim_sdk_0_3;

namespace Core
{
	LocalizationSettings::LocalizationSettings() :
		m_ui(new Ui::LocalizationSettings)
	{
		m_ui->setupUi(this);
		QStringList langs = listThemes("languages");
		foreach (const QString &lang, langs) {
			QLocale locale(lang);
			QString text = QLocale::languageToString(locale.language());
			qDebug() << Q_FUNC_INFO << lang << locale.name() << text;
			if (locale.country() != QLocale::AnyCountry) {
				text += QString(QLatin1Literal(" (")
						 % QLocale::countryToString(locale.country())
						 % QLatin1Literal(")"));
			}
			QListWidgetItem *item = new QListWidgetItem(text, m_ui->languagesList);
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
		QString lang = Config().group("localization").value("lang", QString());
		m_ui->languagesList->setCurrentItem(m_items.value(lang, 0));
	}

	void LocalizationSettings::saveImpl()
	{
		if (QListWidgetItem *item = m_ui->languagesList->currentItem()) {
			Config().group("localization").setValue("lang", item->data(Qt::UserRole));
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
