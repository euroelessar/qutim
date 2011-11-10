/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "jproxymanager.h"
#include "protocol/account/jaccount.h"
#include "protocol/jprotocol.h"
#include <qutim/dataforms.h>
#include <jreen/client.h>

namespace Jabber {

JProxyManager::JProxyManager() :
	NetworkProxyManager(JProtocol::instance())
{
}

QList<NetworkProxyInfo*> JProxyManager::proxies()
{
	static QList<NetworkProxyInfo*> list;
	if (list.isEmpty()) {
		list << Socks5ProxyInfo::instance()
			 << HttpProxyInfo::instance();
	}
	return list;
}

void JProxyManager::setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings)
{
	Q_UNUSED(settings);
	Q_UNUSED(proxy);
	Q_ASSERT(qobject_cast<JAccount*>(account));
//	JAccount *acc = static_cast<JAccount*>(account);
	//acc->connection()->setProxy(toNetworkProxy(settings));
}

} // namespace Jabber

