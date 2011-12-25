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
#include "mprisplayer.h"
#include <QVariantMap>
#include <QFileInfo>
#include <QTime>

namespace qutim_sdk_0_3 {

namespace nowplaying {
	
	MprisPlayer::MprisPlayer(const QString &id) : m_service(id)
	{
		m_version = id.startsWith(QLatin1String("org.mpris.MediaPlayer2.")) ? 2 : 1;
	}

	void MprisPlayer::init()
	{
        qDBusRegisterMetaType<DBusMprisPlayerStatus>();
		if (m_version == 1) {
			m_dbus_interface = new QDBusInterface(m_service, "/Player",
												  "org.freedesktop.MediaPlayer",
												  QDBusConnection::sessionBus(), this);
		} else  if (m_version == 2) {
			m_dbus_interface = new QDBusInterface(m_service, "/org/freedesktop/MediaPlayer2/Player",
												  "org.mpris.MediaPlayer2.Player",
												  QDBusConnection::sessionBus(), this);
		}
    }
	
	void MprisPlayer::requestState()
	{
		QScopedPointer<QDBusPendingCall> call;
		if (m_version == 1) {
			call.reset(new QDBusPendingCall(m_dbus_interface->asyncCall("GetStatus")));
		} else if (m_version == 2) {
			QDBusMessage msg = requestPropertyMessage("PlaybackStatus");
			call.reset(new QDBusPendingCall(QDBusConnection::sessionBus().asyncCall(msg)));
		}
		QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(*call, this);
		connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
				this, SLOT(onStatusChanged(QDBusPendingCallWatcher*)));
	}
	
	void MprisPlayer::requestTrackInfo()
	{
		QScopedPointer<QDBusPendingCall> call;
		if (m_version == 1) {
			call.reset(new QDBusPendingCall(m_dbus_interface->asyncCall("GetMetadata")));
		} else if (m_version == 2) {
			QDBusMessage msg = requestPropertyMessage("Metadata");
			call.reset(new QDBusPendingCall(QDBusConnection::sessionBus().asyncCall(msg)));
		}
		QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(*call, this);
		connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
				this, SLOT(onTrackChanged(QDBusPendingCallWatcher*)));
	}

	void MprisPlayer::startWatching()
	{
        m_is_playing = false;
		QDBusConnection bus = QDBusConnection::sessionBus();
		if (m_version == 1) {
			bus.connect(m_service,
						"/Player",
						"org.freedesktop.MediaPlayer",
						"TrackChange",
						this, SLOT(onTrackChanged(QVariantMap)));
			bus.connect(m_service,
						"/Player",
						"org.freedesktop.MediaPlayer",
						"StatusChange",
						this, SLOT(statusChanged(DBusMprisPlayerStatus)));
		} else if (m_version == 2) {
			bus.connect(m_service,
						QLatin1String("/org/mpris/MediaPlayer2"),
						QLatin1String("org.freedesktop.DBus.Properties"),
						QLatin1String("PropertiesChanged"),
						this, SLOT(onPropertiesChanged(QDBusMessage)));
		}
    }

	void MprisPlayer::stopWatching()
	{
		QDBusConnection bus = QDBusConnection::sessionBus();
		if (m_version == 1) {
			bus.disconnect(m_service,
						   "/Player",
						   "org.freedesktop.MediaPlayer",
						   "TrackChange",
						   this, SLOT(onTrackChanged(QVariantMap)));
			bus.disconnect(m_service,
						   "/Player",
						   "org.freedesktop.MediaPlayer",
						   "StatusChange",
						   this, SLOT(statusChanged(DBusMprisPlayerStatus)));
		} else if (m_version == 2) {
			bus.disconnect(m_service,
						   QLatin1String("/org/mpris/MediaPlayer2"),
						   QLatin1String("org.freedesktop.DBus.Properties"),
						   QLatin1String("PropertiesChanged"),
						   this, SLOT(onPropertiesChanged(QDBusMessage)));
		}
    }
	
	QDBusMessage MprisPlayer::requestPropertyMessage(const QString &property)
	{
		QDBusMessage msg;
		msg = QDBusMessage::createMethodCall(m_service,
											 QLatin1String("/org/mpris/MediaPlayer2"),
											 QLatin1String("org.freedesktop.DBus.Properties"),
											 QLatin1String("Get"));
		msg.setArguments(QVariantList()
						 << QLatin1String("org.mpris.MediaPlayer2.Player")
						 << property);
		return msg;
	}
	
	TrackInfo MprisPlayer::convertInfo(const QVariantMap &map)
	{
        TrackInfo info;
		bool ok;
		if (m_version == 1) {
			info.album = map.value("album").toString();
			info.artist = map.value("artist").toString();
			info.title = map.value("title").toString();
			info.location = QUrl::fromEncoded(map.value("location").toString().toLatin1());
			info.trackNumber = map.value("tracknumber").toInt(&ok);
			if (!ok)
				info.trackNumber = -1;
			info.time = map.value("time").toLongLong(&ok);
			if (!ok)
				info.time = -1;
		} else if (m_version == 2) {
			info.album = map.value("xesam:album").toString();
			info.artist = map.value("xesam:artist").toString();
			info.title = map.value("xesam:title").toString();
			info.location = QUrl::fromEncoded(map.value("xesam:url").toString().toLatin1());
			info.trackNumber = map.value("xesam:trackNumber").toInt(&ok);
			if (!ok)
				info.trackNumber = -1;
			info.time = map.value("mpris:length").toLongLong(&ok) / 1000000;
			if (!ok)
				info.time = -1;
		}
		return info;
	}
	
	void MprisPlayer::onTrackChanged(QDBusPendingCallWatcher *watcher)
	{
		watcher->deleteLater();
		TrackInfo info;
		if (m_version == 1) {
			QDBusPendingReply<QVariantMap> reply = *watcher;
			info = convertInfo(reply.argumentAt<0>());
		} else if (m_version == 2) {
			QDBusPendingReply<QDBusVariant> reply = *watcher;
			QDBusArgument arg = reply.argumentAt<0>().variant().value<QDBusArgument>();
			info = convertInfo(qdbus_cast<QVariantMap>(arg));
		}
		TrackInfoEvent event(info);
		qApp->sendEvent(this, &event);
	}
	
	void MprisPlayer::onStatusChanged(QDBusPendingCallWatcher *watcher)
	{
		watcher->deleteLater();
		bool is_playing = false;
		if (m_version == 1) {
			QDBusPendingReply<DBusMprisPlayerStatus> reply = *watcher;
			is_playing = reply.argumentAt<0>().Play == 0;
		} else if (m_version == 2) {
			QDBusPendingReply<QDBusVariant> reply = *watcher;
			is_playing = reply.argumentAt<0>().variant().toString() == QLatin1String("Playing");
		}
		m_is_playing = is_playing;
		StateEvent event(m_is_playing);
		qApp->sendEvent(this, &event);
	}

	void MprisPlayer::onTrackChanged(const QVariantMap &map)
	{
        TrackInfo info = convertInfo(map);
        if (!info.location.isEmpty() && info.time > 0) {
			TrackInfoEvent event(info);
			qApp->sendEvent(this, &event);
        }
    }

    void MprisPlayer::onStatusChanged(const DBusMprisPlayerStatus &status)
	{
        bool is_playing = status.Play == 0;
		if (m_is_playing != is_playing) {
			m_is_playing = is_playing;
			StateEvent event(m_is_playing);
			qApp->sendEvent(this, &event);
		}
    }
	
	void MprisPlayer::onPropertiesChanged(const QDBusMessage &msg)
	{
		QDBusArgument arg = msg.arguments().at(1).value<QDBusArgument>();
		QVariantMap map = qdbus_cast<QVariantMap>(arg);
		QVariantMap::const_iterator it = map.find(QLatin1String("Metadata"));
		if (it != map.end()) {
			arg = it.value().value<QDBusArgument>();
			onTrackChanged(qdbus_cast<QVariantMap>(arg));
		}
		it = map.find(QLatin1String("PlaybackStatus"));
		if (it != map.end()) {
			DBusMprisPlayerStatus status;
			status.Play = it.value().toString() == QLatin1String("Playing") ? 0 : 1;
			onStatusChanged(status);
		}
	}
} }

const QDBusArgument & operator<<(QDBusArgument &arg, const DBusMprisPlayerStatus& status)
{
    arg.beginStructure();
    arg << status.Play << status.Random << status.Repeat << status.RepeatPlaylist;
    arg.endStructure();
    return arg;
}

const QDBusArgument & operator>>(const QDBusArgument& arg, DBusMprisPlayerStatus& status)
{
    arg.beginStructure();
    arg >> status.Play >> status.Random >> status.Repeat >> status.RepeatPlaylist;
    arg.endStructure();
    return arg;
}

