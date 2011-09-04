#include "jproxymanager.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
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
