/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

