/*
	historySettings

	Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef HISTORYSETTINGS_H
#define HISTORYSETTINGS_H

#include "ui_settings.h"

#include <QSettings>
#include <QFileDialog>

class historySettings  : public QWidget
{
	Q_OBJECT;

public:
	historySettings(QString);
	~historySettings();
	void saveSettings();

signals:
	void settingsChanged();

private:
	Ui::historySettingsClass ui;
	QString m_profile_name;

private slots:


};
#endif
