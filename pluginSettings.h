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

#ifndef W7IPLUGINSETTINGS_H
#define W7IPLUGINSETTINGS_H

#include <QSettings>

#include "ui_settings.h"

class w7isettings  : public QWidget
{
	Q_OBJECT;

public:
	w7isettings( QString profileName );
	virtual ~w7isettings();
	void saveSettings();

//private slots:
	
private:
	Ui::w7isettingsClass ui;
	QString m_profile_name;

};

#endif
