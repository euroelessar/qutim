#ifndef JPROXYMANAGER_H
#define JPROXYMANAGER_H

#include <qutim/networkproxy.h>

namespace Jabber {

	using namespace qutim_sdk_0_3;

	class JProxyManager : public QObject, public NetworkProxyManager
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::NetworkProxyManager)
	public:
		JProxyManager();
		QList<NetworkProxyInfo*> proxies();
		void setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings);
	};

} // namespace Jabber

#endif // JPROXYMANAGER_H
