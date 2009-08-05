/*****************************************************************************
	ProxyFactory

	Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "proxyfactory.h"

#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))

#include "include/qutim/plugininterface.h"

#if 0
#  define DEBUG_PROXY qDebug
#else
#  define DEBUG_PROXY if(0) qDebug
#endif

using namespace qutim_sdk_0_2;

ProxyFactory::ProxyFactory()
{
}

ProxyFactory::~ProxyFactory()
{
}

void ProxyFactory::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+SystemsCity::ProfileName(), "profilesettings");
	settings.beginGroup("proxy");
	int proxy_type = settings.value("proxy/proxyType", 0).toInt();
	switch (proxy_type)
	{
		case 0:
			m_proxy.setType(QNetworkProxy::NoProxy);
			break;
		case 1:
			m_proxy.setType(QNetworkProxy::HttpProxy);
			break;
		case 2:
			m_proxy.setType(QNetworkProxy::Socks5Proxy);
			break;
		default:
			m_proxy.setType(QNetworkProxy::NoProxy);
	}
	m_proxy.setHostName(settings.value("proxy/host", "").toString());
	m_proxy.setPort(settings.value("proxy/port", 1).toInt());
	if ( settings.value("proxy/auth", false).toBool() )
	{
		m_proxy.setUser(settings.value("proxy/user", "").toString());
		m_proxy.setPassword(settings.value("proxy/pass", "").toString());
	}
	settings.endGroup();
}

QList<QNetworkProxy> ProxyFactory::queryProxy(const QNetworkProxyQuery &query)
{
	QList<QNetworkProxy> proxies;
	if(query.protocolTag() == "qrc")
	{
		proxies << QNetworkProxy(QNetworkProxy::NoProxy);
	}
	else
	{
		proxies << m_proxy;
		DEBUG_PROXY() << query.queryType() << query.protocolTag() << query.peerHostName() << query.peerPort();
	}
	return proxies;
}

#endif
