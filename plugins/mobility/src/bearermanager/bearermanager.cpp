/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "bearermanager.h"
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>

#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/utils.h>
#include <qutim/settingslayer.h>
#include "managersettings.h"
#include <qutim/notification.h>

#include <QTimer>

using namespace qutim_sdk_0_3;

BearerManager::BearerManager(QObject *parent) :
	QObject(parent),
	m_confManager(new QNetworkConfigurationManager(this)),
	m_item(0)
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "BearerManager"));

	foreach (Protocol *p, Protocol::all()) {
		connect(p, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		connect(p, SIGNAL(accountRemoved(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountRemoved(qutim_sdk_0_3::Account*)));

		foreach (Account *a, p->accounts())
			onAccountCreated(a);
	}

	m_item.reset(new GeneralSettingsItem<ManagerSettings>(Settings::Plugin,
	                                                      Icon("network-wireless"),
	                                                      QT_TRANSLATE_NOOP("Settings","Connection manager")));
	Settings::registerItem(m_item.data());

	connect(m_confManager, SIGNAL(onlineStateChanged(bool)), SLOT(onOnlineStatusChanged(bool)));

	QList<QNetworkConfiguration> list = m_confManager->allConfigurations();
	if (!list.count()) {
		Notification::send(tr("Unable to find any network configuration. "
							  "Perhaps Qt or QtMobility network bearer configured incorrectly. "
							  "Bearer manager will not work properly, refer to your distribution maintainer."));
	}
}

void BearerManager::onOnlineStatusChanged(bool isOnline)
{
	if (!isOnline) {
		m_accountsToConnect.clear();
		m_timer.stop();
	}
	StatusHash::const_iterator it = m_statusHash.constBegin();
	for (; it != m_statusHash.constEnd(); it++) {
		Account *account = it.key();
		Status status = it.value();
		if (isOnline) {
			account->setStatus(status);
		} else {
			status.setType(Status::Offline);
			status.setChangeReason(Status::ByNetworkError);
			account->setStatus(status);
		}
	}
	Notification::send(isOnline ? tr("Network is available!")
								: tr("Network is unreachable!"));
	emit onlineStateChanged(isOnline);
}

void BearerManager::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	Config config = account->config();
	bool autoConnect = config.value("autoConnect", true);
	Status status = autoConnect ?
	                    config.value("lastStatus", Status(Status::Online))
	                  : Status(Status::Offline);
	if (autoConnect && status == Status::Offline)
		status = Status(Status::Online);
	
	debug() << account->id() << "is created with status" << status;

	bool isOnline = isNetworkOnline();

	m_statusHash.insert(account, status);
	if (isOnline && status != Status::Offline)
		account->setStatus(status);

	connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status, qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
}

void BearerManager::onStatusChanged(qutim_sdk_0_3::Status status)
{
	Account *account = sender_cast<Account*>(sender());
	
	if (status == Status::Connecting) {
		status = status.connectingGoal();
		debug() << account->id() << "is connecting";
	}
	
	if (status.changeReason() == Status::ByUser) {
		debug() << account->id() << "changed status to" << status << "by user";
		m_statusHash.insert(account, status);
		account->config().setValue("lastStatus", status);
	} else if (status.changeReason() == Status::ByNetworkError && isNetworkOnline()) {
		debug() << account->id() << "changed status to" << status << "by network error";
		int reconnectTimeout = status.property("reconnectTimeout", 60);
		m_accountsToConnect.insert(account, reconnectTimeout);
		m_timer.start(m_accountsToConnect.secsTo() * 1000, this);
	} else if (!isNetworkOnline()) {
		debug() << account->id() << "changed status to" << status << " because inet is down";
	} else {
		debug() << account->id() << "changed status to" << status << " because" << status.changeReason();
	}
}

bool BearerManager::isNetworkOnline() const
{
	return m_confManager->isOnline()
	        // We don't have any bearer backend
	        || !m_confManager->allConfigurations().count();
}

void BearerManager::onAccountDestroyed(QObject* obj)
{
	onAccountRemoved(static_cast<Account*>(obj));
}

void BearerManager::onAccountRemoved(qutim_sdk_0_3::Account *account)
{
	m_accountsToConnect.remove(account);
	if (m_accountsToConnect.isEmpty())
		m_timer.stop();
	m_statusHash.remove(account);
}

BearerManager::~BearerManager()
{
	Settings::removeItem(m_item.data());
//	StatusHash::const_iterator it = m_statusHash.constBegin();
//	Config cfg;
//	cfg.beginGroup("status");
//	for (; it != m_statusHash.constEnd(); it++) {
//		Account *account = it.key();
//		cfg.setValue(account->id(), QVariant::fromValue(it.value()));
//		debug() << account->id() << it.value() << account->status().icon().name();
//		debug() << cfg.value(account->id(), Status()).icon().name();
//	}
//	cfg.endGroup();
}

void BearerManager::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId()) {
		foreach (Account *account, m_accountsToConnect.takeNearest()) {
			Status status = m_statusHash.value(account);
			debug() << "change status of" << account->id() << "to" << status << "by timeout";
			if (status != Status::Offline)
				account->setStatus(status);
		}
		if (!m_accountsToConnect.isEmpty()) {
			m_timer.start(1000 * m_accountsToConnect.secsTo(), this);
		} else {
			m_timer.stop();
		}
	} else {
		QObject::timerEvent(event);
	}
}

void BearerManager::ReconnectList::remove(Account *account)
{
	for (int i = 0; i < count(); ++i) {
		if (at(i).second == account) {
			removeAt(i);
			--i;
		}
	}
}

void BearerManager::ReconnectList::insert(Account *account, int timeout)
{
	remove(account);
	debug() << account->id() << "reconnect timeout is" << timeout;
	uint time = QDateTime::currentDateTime().addSecs(timeout).toTime_t();
	BearerManager::ReconnectInfo info = qMakePair(time, account);
	Iterator it = qLowerBound(begin(), end(), info);
	QList<BearerManager::ReconnectInfo>::insert(it, info);
}

QList<Account *> BearerManager::ReconnectList::takeNearest()
{
	uint time = QDateTime::currentDateTime().toTime_t();
	QList<Account*> accounts;
	for (int i = 0; i < count(); ++i) {
		if (at(i).first < time + 5) {
			accounts << at(i).second;
			removeAt(i);
			--i;
		} else {
			break;
		}
	}
	return accounts;
}

bool BearerManager::ReconnectList::isEmpty() const
{
	return QList<BearerManager::ReconnectInfo>::isEmpty();
}

int BearerManager::ReconnectList::secsTo() const
{
	uint time = QDateTime::currentDateTime().toTime_t();
	return isEmpty() ? -1 : int(qint64(at(0).first) - qint64(time));
}

void BearerManager::ReconnectList::clear()
{
	QList<BearerManager::ReconnectInfo>::clear();
}

