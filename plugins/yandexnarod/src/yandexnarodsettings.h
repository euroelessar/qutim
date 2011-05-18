/****************************************************************************
 *  yandexnarodsettings.h
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
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

#define VERSION "0.1.4"

#ifndef YANDEXNARODSETTINGS_H
#define YANDEXNARODSETTINGS_H

#include "ui_yandexnarodsettings.h"
#include "yandexnarodauthorizator.h"
#include <qutim/settingswidget.h>

class YandexNarodSettings  : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT;

public:
	YandexNarodSettings();
	~YandexNarodSettings();
	QString getLogin() { return ui.editLogin->text(); }
	QString getPasswd() { return ui.editPasswd->text(); }
	void btnTest_enabled(bool b) { ui.btnTest->setEnabled(b); }

	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

public slots:
	void onTestClick();
	void authorizationResult(YandexNarodAuthorizator::Result result, const QString &error);

private:
	YandexNarodAuthorizator *m_authorizator;
	Ui::YandexNarodSettingsClass ui;

signals:
	void testclick();

};
#endif
