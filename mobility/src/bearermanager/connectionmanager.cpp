#include "connectionmanager.h"

#include <QNetworkConfigurationManager>

#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/debug.h>
#include "managersettings.h"
#include <qutim/icon.h>
#include <qutim/notificationslayer.h>
#include <qutim/utils.h>
#include <QTimer>

namespace ConnectionManager
{
void changeState(Account *account, bool isOnline)
{
	Config cfg = account->config();
	bool auto_connect = cfg.value("autoConnect",true);
	if (isOnline) {
		if (auto_connect) {
			Status status = account->status();
			cfg.beginGroup("lastStatus");
			status.setType(cfg.value("type", Status::Online));
			status.setSubtype(cfg.value("subtype", 0));
			cfg.endGroup();
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
			PLUGIN_VERSION(0, 2, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
}

bool ConnectionManager::load()
{
	m_network_conf_manager = new QNetworkConfigurationManager(this);
	connect(m_network_conf_manager.data(),SIGNAL(onlineStateChanged(bool)),SLOT(onOnlineStateChanged(bool)));

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
	m_network_conf_manager->deleteLater();
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
	changeState(account,m_network_conf_manager->isOnline());
	connect(account,SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this,SLOT(onStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void ConnectionManager::onStatusChanged(qutim_sdk_0_3::Status now, qutim_sdk_0_3::Status old)
{
	Status::ChangeReason reason = static_cast<Status::ChangeReason>(now.property("changeReason",static_cast<int>(Status::ByUser)));
	Account *a = qobject_cast<Account*>(sender());
	Q_ASSERT(a);

	bool needReconnect = false;
	switch(reason) {
	case Status::ByAuthorizationFailed:
	case Status::ByNetworkError:
		needReconnect = true;
		break;
	default:
		break;
	}

	if(needReconnect) {
		int timeout = now.property("reconnectTimeout", 15);

		QTimer *statusTimer = getTimer(a);
		old.setProperty("changeReason",Status::ByUser);
		old.setProperty("reconnectTimeout", timeout *2);
		statusTimer->setProperty("status",qVariantFromValue(old));
		connect(statusTimer, SIGNAL(timeout()), SLOT(onStatusChangeTimeout()));
		statusTimer->setSingleShot(true);
		statusTimer->start(timeout*1000);

		QString timeoutStr = timeout ? tr("within %1 seconds").arg(timeout) :
									   tr("immediately");

		now.setType(Status::Connecting);
		now.setProperty("reconnectTimeout", 2 *timeout);
		now.setProperty("changeReason", Status::ByIdle);
		a->setStatus(now);

		Notifications::send(Notification::System,this,
							tr("%1 will be reconnected %2").arg(a->name(),timeoutStr),
							tr("ConnectionManager"));
	} else {
		if (a->status().property("changeReason", 0) == Status::ByUser) {
			QTimer *timer = getTimer(a, false);
			if(timer)
				removeTimer(timer);
		}
	}

	if (now.type() != Status::Connecting) {
		Config cfg = a->config("lastStatus");
		cfg.setValue("type", now.type());
		cfg.setValue("subtype", now.subtype());
	}
}

void ConnectionManager::onStatusChangeTimeout()
{
	QTimer *timer = sender_cast<QTimer*>(sender());
	Status status = timer->property("status").value<Status>();
	Account *account = m_timers.key(timer);
	if (account)
		account->setStatus(status);
	removeTimer(timer);
}


QTimer *ConnectionManager::getTimer(Account *account, bool create)
{
	if (!m_timers.contains(account) && create) {
		QTimer *timer = new QTimer(this);
		m_timers.insert(account, timer);
	}
	return m_timers.value(account);
}

void ConnectionManager::removeTimer(QTimer *timer)
{
	m_timers.remove(m_timers.key(timer));
	timer->stop();
	timer->deleteLater();
}

}

QUTIM_EXPORT_PLUGIN(ConnectionManager::ConnectionManager)
