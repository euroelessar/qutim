/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef MPRISPLAYER_H
#define MPRISPLAYER_H

#include "../../include/player.h"
#include <QtDBus>
#include <QTimer>

struct DBusMprisPlayerStatus
{
    int Play; //Playing = 0, Paused = 1, Stopped = 2
    int Random; //Linearly = 0, Randomly = 1
    int Repeat; //Go_To_Next = 0, Repeat_Current = 1
    int RepeatPlaylist; //Stop_When_Finished = 0, Never_Give_Up_Playing = 1
};

Q_DECLARE_METATYPE(DBusMprisPlayerStatus)

namespace qutim_sdk_0_3 {

namespace nowplaying {

	class MprisPlayer : public QObject, public Player
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::nowplaying::Player)
	public:
		MprisPlayer(const QString &id);
		void init();
		void requestState();
		void requestTrackInfo();
		void startWatching();
		void stopWatching();
	private:
		QDBusMessage requestPropertyMessage(const QString &property);
		TrackInfo convertInfo(const QVariantMap &info);
		int m_version;
		QString m_service;
		bool m_is_playing;
		QDBusInterface* m_dbus_interface;
	public slots:
		void onTrackChanged(QDBusPendingCallWatcher *watcher);
		void onStatusChanged(QDBusPendingCallWatcher *watcher);
		void onPropertiesChanged(const QDBusMessage &msg);
		void onTrackChanged(const QVariantMap &info);
		void onStatusChanged(const DBusMprisPlayerStatus &status);
	};
} }

#endif // MPRISPLAYER_H

