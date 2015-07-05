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

