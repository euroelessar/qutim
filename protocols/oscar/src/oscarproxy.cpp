/****************************************************************************
 *  oscarproxy.cpp
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

#include "oscarproxy.h"
#include "icqprotocol.h"
#include "icqaccount.h"
#include <qutim/dataforms.h>

namespace qutim_sdk_0_3 {
namespace oscar {

OscarProxyManager::OscarProxyManager() :
	NetworkProxyManager(IcqProtocol::instance())
{

}

QList<NetworkProxyInfo*> OscarProxyManager::proxies()
{
	static QList<NetworkProxyInfo*> list;
	if (list.isEmpty()) {
		list << Socks5ProxyInfo::instance()
			<< HttpProxyInfo::instance();
	}
	return list;
}

void OscarProxyManager::setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings)
{
	Q_UNUSED(proxy);
	Q_ASSERT(qobject_cast<IcqAccount*>(account));
	IcqAccount *acc = static_cast<IcqAccount*>(account);
	acc->setProxy(toNetworkProxy(settings));
}

} } // namespace qutim_sdk_0_3::oscar
