/****************************************************************************
 *  proxysettings.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef PROXYSETTINGS_H
#define PROXYSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/startupmodule.h>
#include <qutim/dataforms.h>

class QStackedLayout;
class QFormLayout;
class QComboBox;

namespace qutim_sdk_0_3 {
	class Account;
	class NetworkProxyInfo;
}

namespace Core {

using namespace qutim_sdk_0_3;

class ProxySettingsWidget: public SettingsWidget
{
	Q_OBJECT
public:
	ProxySettingsWidget();
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
	virtual void setController(QObject *controller);
private slots:
	void typeChanged(int index);
private:
	QList<NetworkProxyInfo*> proxies();
	QStackedLayout *m_settingsLayout;
	QFormLayout *m_layout;
	QComboBox *m_typeBox;
	Account *m_account;
};

class ProxySettings : public QObject, public qutim_sdk_0_3::StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::StartupModule)
public:
    ProxySettings();
};

} // namespace Core

#endif // PROXYSETTINGS_H
