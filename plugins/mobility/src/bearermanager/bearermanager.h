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

namespace Ureen {
class Account;
class SettingsItem;
}

typedef QHash<Ureen::Account*, Ureen::Status> StatusHash;

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

	void timerEvent(QTimerEvent *event);

signals:
	void onlineStateChanged(bool isOnline);

private slots:
	void onOnlineStatusChanged(bool isOnline);
	void onAccountCreated(Ureen::Account *account);
	void onAccountRemoved(Ureen::Account *account);
	void onAccountDestroyed(QObject *obj);
	void onStatusChanged(Ureen::Status status);

private:	
	bool isNetworkOnline() const;
	void changeStatus(Ureen::Account *a,
					  const Ureen::Status::Type &status);

	typedef QPair<uint, Ureen::Account*> ReconnectInfo;

	class ReconnectList : private QList<ReconnectInfo>
	{
	public:
		void remove(Ureen::Account *account);
		void insert(Ureen::Account *account, int timeout);

		QList<Ureen::Account*> takeNearest();
		bool isEmpty() const;
		int secsTo() const;
		void clear();
	};

	QBasicTimer m_timer;
	bool m_isOnline;
	QNetworkConfigurationManager *m_confManager;
	StatusHash m_statusHash;
	ReconnectList m_accountsToConnect;
	QScopedPointer<Ureen::SettingsItem> m_item;
};

#endif // BEARERMANAGER_H

