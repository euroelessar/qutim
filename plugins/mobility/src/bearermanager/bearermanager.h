/****************************************************************************
 *  bearermanager.h
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

#ifndef BEARERMANAGER_H
#define BEARERMANAGER_H

#include <QObject>
#include <QHash>
#include <qutim/status.h>


namespace qutim_sdk_0_3 {
class Account;
}

typedef QHash<qutim_sdk_0_3::Account*, qutim_sdk_0_3::Status::Type> StatusHash;

class ManagerSettings;
class QNetworkConfigurationManager;
class BearerManager : public QObject
{
    Q_OBJECT
	Q_CLASSINFO("Service", "Bearer")
	Q_CLASSINFO("Uses", "SettingsLayer")
public:
    explicit BearerManager(QObject *parent = 0);
	virtual ~BearerManager();
signals:
	void onlineStateChanged(bool isOnline);
private slots:
	void onOnlineStatusChanged(bool isOnline);
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountRemoved(qutim_sdk_0_3::Account *account);
	void onAccountDestroyed(QObject *obj);
	void onStatusChanged(const qutim_sdk_0_3::Status &status);
private:	
	void changeStatus(qutim_sdk_0_3::Account *a, bool isOnline,
					  const qutim_sdk_0_3::Status::Type &status);

	QNetworkConfigurationManager *m_confManager;
	StatusHash m_statusHash;
	
};

#endif // BEARERMANAGER_H
