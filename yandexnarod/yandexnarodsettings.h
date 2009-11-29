/*
    yandexnarodSettings

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

#define VERSION "0.1.4"

#ifndef YANDEXNARODSETTINGS_H
#define YANDEXNARODSETTINGS_H

#include "ui_yandexnarodsettings.h"
#include <QSettings>

class yandexnarodSettings  : public QWidget
{
	Q_OBJECT;

public:
	yandexnarodSettings(QString);
	~yandexnarodSettings();
	QString getLogin() { return ui.editLogin->text(); }
	QString getPasswd() { return ui.editPasswd->text(); }
	void btnTest_enabled(bool b) { ui.btnTest->setEnabled(b); }

public slots:
	void setStatus(QString str);
	void saveSettings();

private:
	Ui::yandexnarodSettingsClass ui;
	QString m_profile_name;

signals:
	void testclick();

};
#endif
