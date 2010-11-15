#include "connectionmanager.h"

#ifdef HAVE_NETWORK_MANAGER
#	include <QNetworkConfigurationManager>
#endif

#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/debug.h>
#include "managersettings.h"
#include <qutim/icon.h>
#include <qutim/notificationslayer.h>
#include <QTimer>

namespace ConnectionManager
{
void changeState(Account *account, bool isOnline)
{
	bool auto_connect = account->config().value("autoConnect",true);
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
}

bool ConnectionManager::load()
{
#ifdef HAVE_NETWORK_MANAGER
	m_network_conf_manager = new QNetworkConfigurationManager(this);
	connect(m_network_conf_manager.data(),SIGNAL(onlineStateChanged(bool)),SLOT(onOnlineStateChanged(bool)));
#endif

	foreach (Protocol *protocol,Protocol::all()) {
		connect(protocol,
				SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach (Account *a,protocol->accounts()) {
			onAccountCreated(a);
		}
	}

	m_item = new GeneralSettingsItem<ManagerSettings>(Settings::Plugin, Icon("network-wireless"), QT_TRANSLATE_NOOP("Settings","Connection manager"));
	Settings::registerItem(m_item);
	return true;
}

bool ConnectionManager::unload()
{
#ifdef HAVE_NETWORK_MANAGER
	m_network_conf_manager->deleteLater();
#endif
	Settings::removeItem(m_item);
	return true;
}

void ConnectionManager::onOnlineStateChanged(bool isOnline)
{
	foreach (Protocol *protocol,Protocol::all()) {
		foreach (Account *account,protocol->accounts())
			changeState(account,isOnline);
	}
}

void ConnectionManager::onAccountCreated(qutim_sdk_0_3::Account *account)
{
#ifdef HAVE_NETWORK_MANAGER
	changeState(account,m_network_conf_manager->isOnline());
#else
	changeState(account,true);
#endif
	connect(account,SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this,SLOT(onStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void ConnectionManager::onStatusChanged(qutim_sdk_0_3::Status now, qutim_sdk_0_3::Status old)
{
	Status::ChangeReason reason = static_cast<Status::ChangeReason>(now.property("changeReason",static_cast<int>(Status::ByUser)));
	if(now.type() == Status::Offline && reason == Status::ByNetworkError) {
		Account *a = qobject_cast<Account*>(sender());
		Q_ASSERT(a);
		int timeout = now.property("reconnectTimeout",5);

		QTimer *statusTimer = new QTimer(this);
		statusTimer->setProperty("account",qVariantFromValue(a));
		statusTimer->setProperty("status",qVariantFromValue(old));
		old.setProperty("changeReason",Status::ByUser);
		connect(statusTimer,SIGNAL(timeout()),SLOT(onStatusChangeTimeout()));
		statusTimer->setSingleShot(true);
		statusTimer->start(timeout*1000);

		QString timeoutStr = timeout ? tr("within %1 seconds").arg(timeout) :
									   tr("immediately");

		Notifications::send(Notifications::System,this,
							tr("%1 will be reconnected %2").arg(a->name(),timeoutStr),
							tr("ConnectionManager"));
	}
}

void ConnectionManager::onStatusChangeTimeout()
{
	QObject *timer = sender();
	Status status = timer->property("status").value<Status>();
	Account *account = timer->property("account").value<Account*>();
	Q_ASSERT(account);
	account->setStatus(status);
	timer->deleteLater();
}

}

QUTIM_EXPORT_PLUGIN(ConnectionManager::ConnectionManager)
