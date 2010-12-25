/****************************************************************************
 *  ircproxymanager.cpp
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

#include "ircproxymanager.h"
#include "ircaccount_p.h"
#include "ircconnection.h"
#include "ircprotocol.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcProxyManager::IrcProxyManager() :
	NetworkProxyManager(IrcProtocol::instance())
{
}

QList<NetworkProxyInfo*> IrcProxyManager::proxies()
{
	static QList<NetworkProxyInfo*> list;
	if (list.isEmpty()) {
		list << Socks5ProxyInfo::instance()
			<< HttpProxyInfo::instance();
	}
	return list;
}

void IrcProxyManager::setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings)
{
	Q_UNUSED(proxy);
	Q_ASSERT(qobject_cast<IrcAccount*>(account));
	IrcAccount *acc = static_cast<IrcAccount*>(account);
	acc->d->conn->socket()->setProxy(toNetworkProxy(settings));
}

} } // namespace qutim_sdk_0_3::irc
