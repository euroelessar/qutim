/*
    imagepubSettings

		Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef IMAGEPUBSETTINGS_H
#define IMAGEPUBSETTINGS_H

#include "ui_imagepubsettings.h"

#include <QSettings>
#include <QFile>

class imagepubSettings  : public QWidget
{
	Q_OBJECT;

public:
	imagepubSettings(QString);
	~imagepubSettings();
	void saveSettings();

private:
	Ui::imagepubSettingsClass ui;
	QString m_profile_name;

};
#endif
