/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef QUETZALACCOUNTSETTINGS_H
#define QUETZALACCOUNTSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/dataforms.h>
#include <purple.h>

class QuetzalAccount;
class QVBoxLayout;

class QuetzalAccountSettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
public:
    QuetzalAccountSettings();
	virtual ~QuetzalAccountSettings();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
	virtual void setController(QObject *controller);
	void setAccount(PurpleAccount *account, PurplePlugin *prpl);
protected slots:
	void onChanged();
private:
	QVariant fromOption(PurpleAccountOption *option);
	
	QScopedPointer<qutim_sdk_0_3::AbstractDataForm> m_form;
	QVBoxLayout *m_layout;
	PurpleAccount *m_account;
	PurplePluginProtocolInfo *m_info;
};

#endif // QUETZALACCOUNTSETTINGS_H
