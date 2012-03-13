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
	foreach (QNetworkConfiguration conf, list) {
		debug() << conf.bearerName();
	}
	if (!list.count()) {
		Notification::send(tr("Unable to find any network configuration. "
							  "Perhaps Qt or QtMobility network bearer configured incorrectly. "
							  "Bearer manager will not work properly, refer to your distribution maintainer."));
	}
}

void BearerManager::onOnlineStatusChanged(bool isOnline)
{
	StatusHash::const_iterator it = m_statusHash.constBegin();
	for (; it != m_statusHash.constEnd(); it++) {
		Account *account = it.key();
		Status status = it.value();
		if (isOnline)
			account->setStatus(status);
		else {
			account->blockSignals(true);
			status.setType(Status::Offline);
			status.setProperty("changeReason", Status::ByNetworkError);
			account->setStatus(status);
			account->blockSignals(false);
		}
	}
	Notification::send(isOnline ? tr("Network is available!")
								: tr("Network is unreachable!"));
	emit onlineStateChanged(isOnline);
}

void BearerManager::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	//simple spike for stupid distros!
	bool isOnline = m_confManager->isOnline();
	if (!m_confManager->allConfigurations().count())
		isOnline = true;

	bool autoConnect = account->config().value("autoConnect", true);
	Status status = account->status();
	status.setType(autoConnect && isOnline ? Status::Online : Status::Offline);
	account->setStatus(status);
	connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status, qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
}

void BearerManager::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = sender_cast<Account*>(sender());
	Status::ChangeReason reason = status.property("changeReason", Status::ByUser);
	if (reason == Status::ByUser)
		m_statusHash.insert(account, status.type());
}

void BearerManager::onAccountDestroyed(QObject* obj)
{
	onAccountRemoved(static_cast<Account*>(obj));
}

void BearerManager::onAccountRemoved(qutim_sdk_0_3::Account *account)
{
	m_statusHash.remove(account);
}

BearerManager::~BearerManager()
{
	Settings::removeItem(m_item.data());
}

