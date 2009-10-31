/****************************************************************************
 * kdespellersettings.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
#include "ui_kdespellersettings.h"
#include <QSettings>

KdeSpellerSettings::KdeSpellerSettings(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::KdeSpellerSettings)
{
	m_ui->setupUi(this);
	connect( m_ui->dictionaryComboBox, SIGNAL(dictionaryChanged(QString)), this, SIGNAL(settingsChanged()) );
	connect( m_ui->autodetect, SIGNAL(stateChanged(int)), this, SIGNAL(settingsChanged()) );
}

KdeSpellerSettings::~KdeSpellerSettings()
{
    delete m_ui;
}

QString KdeSpellerSettings::suggestLanguage( QString lang, Speller *speller )
{
	QStringList langs = speller->availableLanguages();
	if( langs.contains( lang ) )
		return lang;
	lang = QLocale::system().name();
	if( langs.contains( lang ) )
		return lang;
	lang = lang.section( '_', 0, 0 );
	if( langs.contains( lang ) )
		return lang;
	return QString();
}

void KdeSpellerSettings::loadSettings( const QString &profile_name, Speller *speller )
{
	m_profile_name = profile_name;
	QSettings settings( QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings" );
	settings.beginGroup( "speller" );
	m_ui->autodetect->setChecked( settings.value( "autodetect", false ).toBool() );
	QString lang = KdeSpellerSettings::suggestLanguage( settings.value( "language", QString() ).toString(), speller );
	if( !lang.isEmpty() )
		m_ui->dictionaryComboBox->setCurrentByDictionary( lang );
	settings.endGroup();
}

void KdeSpellerSettings::saveSettings()
{
	QSettings settings( QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings" );
	settings.beginGroup( "speller" );
	settings.setValue( "autodetect", m_ui->autodetect->isChecked() );
	settings.setValue( "language", m_ui->dictionaryComboBox->currentDictionary() );
	settings.endGroup();
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
