/****************************************************************************
 *  bearermanager.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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

void BearerManager::changeStatus(Account *a, bool isOnline, const qutim_sdk_0_3::Status &s)
{
	Q_UNUSED(s);
	Config cfg = a->config();
	bool auto_connect = cfg.value("autoConnect",false);
	qDebug()<<"dkjdsbsdbsbvn";
	qDebug()<<isOnline<<auto_connect;
	if (isOnline){
		if (auto_connect) {
			Status status = a->status();
			status.setType(Status::Online);
			a->setStatus(status);
		}
	}else {
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
	Config cfg;
	debug() << cfg.value("status");
	cfg.beginGroup("status");
	Status s = cfg.value<Status>(account->id());
	qDebug() << account->id() << s << cfg.value<QByteArray>(account->id());

	//simple spike for stupid distros!

	QList<QNetworkConfiguration> list = m_confManager->allConfigurations();
	foreach (QNetworkConfiguration conf, list) {
		debug() << conf.bearerName();
	}

	bool isOnline = true;
	if (list.count())
		isOnline = m_confManager->isOnline();

	changeStatus(account, isOnline, s);

	connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
		this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
}

void BearerManager::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = sender_cast<Account*>(sender());
	if (status.property("changeReason", Status::ByUser) == Status::ByUser)
		m_statusHash.insert(account, status);
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
	StatusHash::const_iterator it = m_statusHash.constBegin();
	Config cfg;
	cfg.beginGroup("status");
	for (; it != m_statusHash.constEnd(); it++) {
		Account *account = it.key();
		cfg.setValue(account->id(), QVariant::fromValue(it.value()));
		debug() << account->id() << it.value() << account->status().icon().name();
		debug() << cfg.value(account->id(), Status()).icon().name();
	}
	cfg.endGroup();
	cfg.sync();
}
