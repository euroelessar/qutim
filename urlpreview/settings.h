/*
    urlpreviewSettings

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

#ifndef URLPREVIEWSETTINGS_H
#define URLPREVIEWSETTINGS_H

#include "ui_settings.h"

#include <QSettings>
#include <QFileDialog>

class urlpreviewSettings  : public QWidget
{
	Q_OBJECT;

public:
	urlpreviewSettings(QString);
	~urlpreviewSettings();
	void saveSettings();

signals:
	void settingsChanged();

private:
	Ui::urlpreviewSettingsClass ui;
	QString m_profile_name;

private slots:


};
#endif
