/****************************************************************************
 * kdespellerlayer.cpp
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

#include "kdespellerlayer.h"
#include "kdespellersettings.h"

KdeSpellerLayer::KdeSpellerLayer()
{
	m_speller = 0;
}

bool KdeSpellerLayer::init( PluginSystemInterface *plugin_system )
{
	m_speller = new Speller();
	m_name = "kde";
	m_version = "0.1.0";
	m_plugin_system = plugin_system;
	return true;
}

void KdeSpellerLayer::release()
{
	delete m_speller;
	foreach(Highlighter *highlighter, m_highlighters)
	{
		if(highlighter)
			delete highlighter;
	}
	m_speller = 0;
	m_highlighters.clear();
}

void KdeSpellerLayer::setProfileName( const QString &profile_name )
{
	m_profile_name = profile_name;
	loadSettings();
}

void KdeSpellerLayer::setLayerInterface( LayerType type, LayerInterface *layer_interface )
{
	Q_UNUSED(type);
	Q_UNUSED(layer_interface);
}

void KdeSpellerLayer::loadSettings()
{
	QSettings settings( QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim." + m_profile_name, "profilesettings" );
	settings.beginGroup( "speller" );
	m_autodetect = settings.value( "autodetect", false ).toBool();
	QString lang = KdeSpellerSettings::suggestLanguage( settings.value( "language", QString() ).toString(), m_speller );
	if( !lang.isEmpty() )
		m_dictionary = lang;
	else if( !m_speller->availableDictionaries().isEmpty() )
		m_dictionary = m_speller->availableDictionaries().begin().value();
	QMap<QTextEdit *, QPointer<Highlighter> >::iterator it = m_highlighters.begin();
	for( ; it != m_highlighters.end(); it++ )
	{
		if(!it.value())
			continue;
		it.value()->setAutomatic( m_autodetect );
		it.value()->setCurrentLanguage( m_dictionary );
	}
	m_speller->setLanguage( m_dictionary );
	settings.endGroup();
}


void KdeSpellerLayer::saveLayerSettings()
{
	if( !m_settings.isEmpty() )
	{
		SettingsStructure settings = m_settings.first();
		KdeSpellerSettings *widget = static_cast<KdeSpellerSettings *>( settings.settings_widget );
		widget->saveSettings();
		loadSettings();
	}
}

QList<SettingsStructure> KdeSpellerLayer::getLayerSettingsList()
{
	SettingsStructure settings;
	KdeSpellerSettings *widget = new KdeSpellerSettings();
	widget->loadSettings( m_profile_name, m_speller );
	settings.settings_widget = widget;
	settings.settings_item = new QTreeWidgetItem();
	settings.settings_item->setText( 0, tr("Spell checker") );
	settings.settings_item->setIcon( 0, m_plugin_system->getIcon( "spelling" ) );
	m_settings << settings;
	return m_settings;
}

void KdeSpellerLayer::removeLayerSettings()
{
	foreach( const SettingsStructure &settings, m_settings )
	{
		delete settings.settings_item;
		delete settings.settings_widget;
	}
	m_settings.clear();
}

void KdeSpellerLayer::saveGuiSettingsPressed()
{
}

QList<SettingsStructure> KdeSpellerLayer::getGuiSettingsList()
{
	return m_gui_settings;
}

void KdeSpellerLayer::removeGuiLayerSettings()
{
}

void KdeSpellerLayer::startSpellCheck( QTextEdit *document )
{
	if( Highlighter *highlighter = m_highlighters.value( document ) )
	{
		highlighter->setActive( true );
	}
	else
	{
		Highlighter *highlighter = new Highlighter( document );
		highlighter->setAutomatic( m_autodetect );
		highlighter->setCurrentLanguage( m_dictionary );
		m_highlighters.insert( document, QPointer<Highlighter>(highlighter) );
	}
}

void KdeSpellerLayer::stopSpellCheck( QTextEdit *document )
{
	if( Highlighter *highlighter = m_highlighters.value( document ) )
		highlighter->setActive( false );
}

bool KdeSpellerLayer::isCorrect( const QString &word ) const
{
	if( m_speller )
		return m_speller->isCorrect( word );
	return true;
}

bool KdeSpellerLayer::isMisspelled( const QString &word ) const
{
	if( m_speller )
		return m_speller->isMisspelled( word );
	return false;
}

QStringList KdeSpellerLayer::suggest( const QString &word ) const
{
	if( m_speller )
		return m_speller->suggest( word );
	QStringList list;
	list.append( word );
	return list;
}

