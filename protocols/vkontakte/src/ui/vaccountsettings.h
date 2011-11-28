/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef VACCOUNTSETTINGS_H
#define VACCOUNTSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

using namespace qutim_sdk_0_3;

namespace Ui {
	class VAccountSettings;
}
class VAccount;
class VAccountSettings : public SettingsWidget
{
    Q_OBJECT
public:
    explicit VAccountSettings(QWidget *parent = 0);
	virtual ~VAccountSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
	virtual void setController(QObject *controller);
private slots:
	void onPasswdChanged(const QString &passwd);
private:
	Ui::VAccountSettings *ui;
	VAccount *m_account;
};

#endif // VACCOUNTSETTINGS_H

