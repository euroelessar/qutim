/****************************************************************************
**
** qutIM - instant messenger
**
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

