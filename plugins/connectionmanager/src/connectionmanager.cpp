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

ConnectionManager::ConnectionManager()
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "BearerManager"));
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
}

ConnectionManager::~ConnectionManager()
{
#ifdef HAVE_NETWORK_MANAGER
	m_network_conf_manager->deleteLater();
#endif
	Settings::removeItem(m_item);
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
	if (now.type() == Status::Connecting)
		return;

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
		if (old == Status::Connecting) {
			Config cfg = a->config("lastStatus");
			old.setType(cfg.value("type", Status::Online));
			old.setSubtype(cfg.value("subtype", 0));
			if (old.type() == Status::Offline)
				old.setType(Status::Online);
		}
		statusTimer->setProperty("status",qVariantFromValue(old));
		connect(statusTimer, SIGNAL(timeout()), SLOT(onStatusChangeTimeout()));
		statusTimer->setSingleShot(true);
		statusTimer->start(timeout*1000);

		QString timeoutStr = timeout ? tr("within %n seconds", "reconnect at", timeout)
		                             : tr("immediately");

		now.setType(Status::Connecting);
		now.setProperty("reconnectTimeout", 2 *timeout);
		now.setProperty("changeReason", Status::ByIdle);
		a->setStatus(now);
		
		NotificationRequest request(Notification::System);
		request.setObject(this);
		request.setText(tr("%1 will be reconnected %2").arg(a->name(),timeoutStr));
		request.send();
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
	qDebug() << Q_FUNC_INFO << account << status;
	if (account)
		account->setStatus(status);
	removeTimer(timer);
}


QTimer *ConnectionManager::getTimer(Account *account, bool create)
{
	if (!m_timers.contains(account) && create) {
		QTimer *timer = new QTimer(this);
		timer->setSingleShot(true);
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

