#include "connectionmanager.h"
#include <QNetworkConfigurationManager>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/debug.h>

namespace ConnectionManager
{

	ConnectionManager::ConnectionManager() :
		m_network_conf_manager(new QNetworkConfigurationManager(this))
	{
		//addAuthor(); //TODO
	}

	ConnectionManager::~ConnectionManager()
	{

	}

	void ConnectionManager::init()
	{
		debug() << Q_FUNC_INFO;
		connect(m_network_conf_manager.data(),SIGNAL(onlineStateChanged(bool)),SLOT(onOnlineStateChanged(bool)));
	}

	bool ConnectionManager::load()
	{
		return true;
	}

	bool ConnectionManager::unload()
	{
		return false;
	}

	void ConnectionManager::onOnlineStateChanged(bool isOnline)
	{
		debug() << "onlineStateChanged" << isOnline;
		foreach (Protocol *protocol,allProtocols()) {
			foreach (Account *account,protocol->accounts()) {
				bool auto_connect = account->config().value("autoConnect",true);
				//TODO
				//Status last_status = account->config().value("lastStatus",Status(Status::Online));
				if (isOnline) {
					if (auto_connect) {
						Status status = account->status();
						status.setType(Status::Online);
						account->setStatus(status);
					}
				}
				else {
					Status status = account->status();
					status.setType(Status::Offline);
					account->setStatus(status);
				}

			}
		}
	}

}

QUTIM_EXPORT_PLUGIN(ConnectionManager::ConnectionManager)
