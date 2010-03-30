/****************************************************************************
 * kdespellersettings.cpp
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

#include "kdespellersettings.h"
#include "kdespellerlayer.h"
#include "ui_kdespellersettings.h"
#include <qutim/configbase.h>

KdeSpellerSettings::KdeSpellerSettings() :
    m_ui(new Ui::KdeSpellerSettings)
{
	m_ui->setupUi(this);
	lookForWidgetState(m_ui->dictionaryComboBox);
	lookForWidgetState(m_ui->autodetect);
}

KdeSpellerSettings::~KdeSpellerSettings()
{
    delete m_ui;
}

QString KdeSpellerSettings::suggestLanguage(QString lang, Speller *speller)
{
	QStringList langs = speller->availableLanguages();
	if (langs.contains(lang))
		return lang;
	lang = QLocale::system().name();
	if (langs.contains(lang))
		return lang;
	lang = lang.section('_', 0, 0);
	if (langs.contains(lang))
		return lang;
	return QString();
}

void KdeSpellerSettings::loadImpl()
{
	Speller *speller = KdeSpellerLayer::spellerInstance();
	ConfigGroup group = Config().group("speller");
	m_ui->autodetect->setChecked(group.value("autodetect", false));
	QString lang = suggestLanguage(group.value("language", QString()), speller);
	if (!lang.isEmpty())
		m_ui->dictionaryComboBox->setCurrentByDictionary(lang);
}

void KdeSpellerSettings::saveImpl()
{
	ConfigGroup group = Config().group("speller");
	group.setValue("autodetect", m_ui->autodetect->isChecked());
	group.setValue("language", m_ui->dictionaryComboBox->currentDictionary());
}

void KdeSpellerSettings::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
