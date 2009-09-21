/*
    W7 integration plugin

    Copyright (c) 2009 by Belov Nikita <null@deltaz.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "pluginSettings.h"

w7isettings::w7isettings( QString profileName ) : m_profile_name( profileName )
{
	ui.setupUi(this);

	QSettings settings( QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "w7isettings" );
	ui.showCLBox->setChecked( settings.value( "main/showcl", true ).toBool() );
}

w7isettings::~w7isettings()
{
}

void w7isettings::saveSettings()
{
	QSettings settings( QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "w7isettings" );
	settings.setValue( "main/showcl", ui.showCLBox->isChecked() );
}
