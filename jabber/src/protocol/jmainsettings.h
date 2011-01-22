/*
*  Copyright (c) 2009 by Denis Daschenko <daschenko@gmail.com>
*  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
*/

#ifndef JABBERSETTINGS_H
#define JABBERSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/configbase.h>
#include <QPointer>

namespace Ui
{
class JMainSettings;
}

namespace Jabber
{
using namespace qutim_sdk_0_3;
class JAccount;
class JMainSettings: public SettingsWidget
{
	Q_OBJECT
public:
	JMainSettings();
	~JMainSettings();
	void loadImpl();
	void cancelImpl();
	void saveImpl();
	virtual void setController(QObject *controller);
private:
	Ui::JMainSettings *ui;
	QPointer<JAccount> m_account;
};
}

#endif // JABBERSETTINGS_H
