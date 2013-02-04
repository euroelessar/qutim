/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#define VERSION "0.1.4"

#ifndef YANDEXNARODSETTINGS_H
#define YANDEXNARODSETTINGS_H

#include "ui_yandexnarodsettings.h"
#include "yandexnarodauthorizator.h"
#include <qutim/settingswidget.h>

class YandexNarodSettings  : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT

public:
	YandexNarodSettings();
	~YandexNarodSettings();
	QString getLogin() { return ui.editLogin->text(); }
	QString getPasswd() { return ui.editPasswd->text(); }

	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

public slots:
	void on_testButton_clicked();
	void authorizationResult(YandexNarodAuthorizator::Result result, const QString &error);

private:
	YandexNarodAuthorizator *m_authorizator;
	Ui::YandexNarodSettingsClass ui;
};
#endif

