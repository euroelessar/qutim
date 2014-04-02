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

#ifndef BEARERMANAGER_H
#define BEARERMANAGER_H

#include <QObject>
#include <QHash>
#include <qutim/status.h>
#include <QScopedPointer>
#include <QBasicTimer>
#include <QDateTime>
#include <plugin.h>

namespace qutim_sdk_0_3 {
class Account;
}

typedef QHash<qutim_sdk_0_3::Account*, qutim_sdk_0_3::Status> StatusHash;

class ManagerSettings;
class QNetworkConfigurationManager;
class BearerManager : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("Service", "Bearer")
	Q_CLASSINFO("Uses", "SettingsLayer")
public:
	explicit BearerManager();
	void init();
	bool load();
	bool unload();

	void timerEvent(QTimerEvent *event);

signals:
	void onlineStateChanged(bool isOnline);

private slots:
	void onOnlineStatusChanged(bool isOnline);
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountRemoved(qutim_sdk_0_3::Account *account);
	void onAccountDestroyed(QObject *obj);
	void onStatusChanged(qutim_sdk_0_3::Status status);

private:	
	bool isNetworkOnline() const;
	void changeStatus(qutim_sdk_0_3::Account *a,
					  const qutim_sdk_0_3::Status::Type &status);

	typedef QPair<uint, qutim_sdk_0_3::Account*> ReconnectInfo;

	QBasicTimer m_timer;
	bool m_isOnline;
	QNetworkConfigurationManager *m_confManager;
	StatusHash m_statusHash;
};

#endif // BEARERMANAGER_H

