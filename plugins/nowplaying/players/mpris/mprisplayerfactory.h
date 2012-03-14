/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "../../include/player.h"
#include <QtDBus>

namespace qutim_sdk_0_3 {

namespace nowplaying {
	
	class MprisPlayerFactory : public QObject, public PlayerFactory
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::nowplaying::PlayerFactory)
	public:
		MprisPlayerFactory();
		QMap<QString, Player::Info> players();
		QObject *player(const QString &id);
	private slots:
		void onServiceRegistered(const QString &service);
		void onServiceUnregistered(const QString &service);
		void onIdentityReceived(QDBusPendingCallWatcher *watcher);
		void onDesktopNameReceived(QDBusPendingCallWatcher *watcher);
		void onNamesReceived(QDBusPendingCallWatcher *watcher);
	private:
		enum InfoType { Name, DesktopId };
		void ensureServiceInfo(const QString &service, InfoType type);
		QMap<QString, Player::Info> m_knownPlayers;
	};
} }

