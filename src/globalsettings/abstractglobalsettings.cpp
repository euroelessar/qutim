/*
    AbstractGlobalSettings

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "abstractglobalsettings.h"
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
#include "proxyfactory.h"
#endif
#include <QNetworkProxy>
#include <QSettings>
#include <QPointer>

AbstractGlobalSettings::AbstractGlobalSettings()
{
}

AbstractGlobalSettings::~AbstractGlobalSettings()
{
}

AbstractGlobalSettings &AbstractGlobalSettings::instance()
{
	static AbstractGlobalSettings ags;
	return ags;
}

void AbstractGlobalSettings::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;
	loadNetworkSettings();
}

void AbstractGlobalSettings::loadNetworkSettings()
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
	static QPointer<ProxyFactory> factory;
	if(!factory)
	{
		factory = new ProxyFactory;
		QNetworkProxyFactory::setApplicationProxyFactory(factory);
	}
	factory->loadSettings();
#else
	QNetworkProxy global_proxy;
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("proxy");
	int proxy_type = settings.value("proxy/proxyType", 0).toInt();
	switch (proxy_type)
	{
		case 0:
			global_proxy.setType(QNetworkProxy::NoProxy);
			break;
		case 1:
			global_proxy.setType(QNetworkProxy::HttpProxy);
			break;
		case 2:
			global_proxy.setType(QNetworkProxy::Socks5Proxy);
			break;
		default:
			global_proxy.setType(QNetworkProxy::NoProxy);
	}
	global_proxy.setHostName(settings.value("proxy/host", "").toString());
	global_proxy.setPort(settings.value("proxy/port", 1).toInt());
	if ( settings.value("proxy/auth", false).toBool() )
	{
		global_proxy.setUser(settings.value("proxy/user", "").toString());
		global_proxy.setPassword(settings.value("proxy/pass", "").toString());
	}
	settings.endGroup();
	QNetworkProxy::setApplicationProxy(global_proxy);
#endif
}
