/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include <QTimer>

using namespace qutim_sdk_0_3;

BearerManager::BearerManager(QObject *parent) :
	QObject(parent),
	m_confManager(new QNetworkConfigurationManager(this))
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

	GeneralSettingsItem<ManagerSettings> *m_item = new GeneralSettingsItem<ManagerSettings>(Settings::Plugin, Icon("network-wireless"), QT_TRANSLATE_NOOP("Settings","Connection manager"));
	Settings::registerItem(m_item);

	connect(m_confManager, SIGNAL(onlineStateChanged(bool)), SLOT(onOnlineStatusChanged(bool)));
}

void BearerManager::changeStatus(Account *a, bool isOnline, const qutim_sdk_0_3::Status::Type &s)
{
	Q_UNUSED(s);
	Config cfg = a->config();
	bool auto_connect = cfg.value("autoConnect", true);
	if (isOnline){
		if (auto_connect) {
			Status status = a->status();
			status.setType(s == Status::Offline ? Status::Online : s);
			a->setStatus(status);
		}
	} else {
		Status status = a->status();
		status.setType(Status::Offline);
		status.setProperty("changeReason", Status::ByNetworkError);
		a->setStatus(status);
	}
}

void BearerManager::onOnlineStatusChanged(bool isOnline)
{
	StatusHash::const_iterator it = m_statusHash.constBegin();
	for (; it != m_statusHash.constEnd(); it++)
		changeStatus(it.key(), isOnline, it.value());
	emit onlineStateChanged(isOnline);
}

void BearerManager::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	//simple spike for stupid distros!

	QList<QNetworkConfiguration> list = m_confManager->allConfigurations();
	foreach (QNetworkConfiguration conf, list) {
		debug() << conf.bearerName();
	}

	bool isOnline = true;
	if (list.count())
		isOnline = m_confManager->isOnline();

	changeStatus(account, isOnline, Status::Online);

	connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
}

void BearerManager::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = sender_cast<Account*>(sender());
	if (status.property("changeReason", Status::ByUser) == Status::ByUser)
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
}

