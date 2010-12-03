/****************************************************************************
 *  vproxymanager.cpp
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
 ***************************************************************************/

#include "vproxymanager.h"
#include "vaccount.h"
#include "vconnection.h"
#include "vkontakteprotocol.h"

VProxyManager::VProxyManager() :
	NetworkProxyManager(VkontakteProtocol::instance())
{
}

QList<NetworkProxyInfo*> VProxyManager::proxies()
{
	static QList<NetworkProxyInfo*> list;
	if (list.isEmpty()) {
		list << Socks5ProxyInfo::instance()
			<< HttpProxyInfo::instance();
	}
	return list;
}

void VProxyManager::setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings)
{
	Q_UNUSED(proxy);
	Q_ASSERT(qobject_cast<VAccount*>(account));
	VAccount *acc = qobject_cast<VAccount*>(account);
	acc->connection()->setProxy(toNetworkProxy(settings));
}
