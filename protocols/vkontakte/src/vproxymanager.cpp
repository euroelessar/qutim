/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "vproxymanager.h"
#include "vaccount.h"
#include "vprotocol.h"
#include <vreen/connection.h>

using namespace qutim_sdk_0_3;

VProxyManager::VProxyManager() :
	NetworkProxyManager(VProtocol::instance())
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
	VAccount *vaccount = qobject_cast<VAccount*>(account);
	vaccount->connection()->setProxy(toNetworkProxy(settings));
}

