/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2014 Roman Tretyakov <roman@trett.ru>
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
#include <qutim/notification.h>

#include <QTimer>

using namespace qutim_sdk_0_3;

BearerManager::BearerManager() :
	m_confManager(new QNetworkConfigurationManager(this))
{

}

void BearerManager::init()
{
	setInfo(QT_TRANSLATE_NOOP("Service", "BearerManager"),
			QT_TRANSLATE_NOOP("Service", "Connection manager"),
			PLUGIN_VERSION(0, 0, 1, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("Sauron"));
	addAuthor(QLatin1String("trett"));
}

bool BearerManager::load()
{
	m_isOnline = m_confManager->isOnline();

	foreach (Protocol *p, Protocol::all()) {
		connect(p, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		connect(p, SIGNAL(accountRemoved(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountRemoved(qutim_sdk_0_3::Account*)));

		foreach (Account *a, p->accounts())
			onAccountCreated(a);
	}

	connect(m_confManager, SIGNAL(onlineStateChanged(bool)), SLOT(onOnlineStatusChanged(bool)));

	QList<QNetworkConfiguration> list = m_confManager->allConfigurations();
	if (!list.count()) {
		Notification::send(tr("Unable to find any network configuration. "
							  "Perhaps Qt or QtMobility network bearer configured incorrectly. "
							  "Bearer manager will not work properly, refer to your distribution maintainer."));
		return false;
	}
	return true;
}

bool BearerManager::unload()
{
	return true;
}

void BearerManager::onOnlineStatusChanged(bool isOnline)
{
	if (m_isOnline == isOnline)
		return;
	m_isOnline = isOnline;
	if (!isOnline) {
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
	emit onlineStateChanged(isOnline);

}

void BearerManager::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	Config config = account->config();
	Status status = config.value("lastStatus", Status(Status::Online));
	
	qDebug() << account->id() << "is created with status" << status;

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
		qDebug() << account->id() << "is connecting";
	}
	
	if (status.changeReason() == Status::ByUser) {
		qDebug() << account->id() << "changed status to" << status << "by user";
		m_statusHash.insert(account, status);
		account->config().setValue("lastStatus", status);
		return;
	}
	if (!m_confManager->isOnline()) {
		qDebug() << account->id() << "changed status to" << status << "by network error";
		m_timer.start(60000, this);
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
	m_statusHash.remove(account);
}

void BearerManager::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId()) {

		foreach (Protocol *p, Protocol::all()) {
			foreach (Account *a, p->accounts()) {
				Status status = a->config().value("lastStatus", Status(Status::Online));
				qDebug() << "change status of" << a->id() << "to" << status << "by timeout";
				if (status != Status::Offline)
					a->setStatus(status);
			}
		}
		m_timer.start(60000, this);
	}
}

QUTIM_EXPORT_PLUGIN(BearerManager)

