#include "connectionmanager.h"
#include <QNetworkConfigurationManager>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include "managersettings.h"
#include <qutim/icon.h>

namespace ConnectionManager
{
void changeState(Account *account, bool isOnline)
{
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

ConnectionManager::ConnectionManager() :
	m_network_conf_manager(new QNetworkConfigurationManager(this))
{
}

ConnectionManager::~ConnectionManager()
{

}

void ConnectionManager::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "ConnectionManager"),
			QT_TRANSLATE_NOOP("Plugin", "Used to monitor the availability of network."),
			PLUGIN_VERSION(0, 1, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));

	connect(m_network_conf_manager.data(),SIGNAL(onlineStateChanged(bool)),SLOT(onOnlineStateChanged(bool)));
}

bool ConnectionManager::load()
{
	foreach (Protocol *protocol,allProtocols()) {
		connect(protocol,
				SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach (Account *a,protocol->accounts()) {
			onAccountCreated(a);
		}
	}

	GeneralSettingsItem<ManagerSettings> *item = new GeneralSettingsItem<ManagerSettings>(Settings::Plugin, Icon("network-wireless"), QT_TRANSLATE_NOOP("Settings","Connection manager"));
	Settings::registerItem(item);

	return true;
}

bool ConnectionManager::unload()
{
	return false;
}

void ConnectionManager::onOnlineStateChanged(bool isOnline)
{
	foreach (Protocol *protocol,allProtocols()) {
		foreach (Account *account,protocol->accounts())
			changeState(account,isOnline);
	}
}

void ConnectionManager::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	changeState(account,m_network_conf_manager->isOnline());
}

}

QUTIM_EXPORT_PLUGIN(ConnectionManager::ConnectionManager)
