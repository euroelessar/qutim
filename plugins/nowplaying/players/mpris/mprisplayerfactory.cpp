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
#include "mprisplayerfactory.h"
#include "mprisplayer.h"
#include <qutim/icon.h>

namespace qutim_sdk_0_3 {

namespace nowplaying {
	
	MprisPlayerFactory::MprisPlayerFactory()
	{
		QDBusConnection conn = QDBusConnection::sessionBus();
		connect(conn.interface(), SIGNAL(serviceRegistered(QString)),
				SLOT(onServiceRegistered(QString)));
		connect(conn.interface(), SIGNAL(serviceUnregistered(QString)),
				SLOT(onServiceUnregistered(QString)));
		QDBusPendingCall call = conn.interface()->asyncCall("ListNames");
		QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
		connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
				this, SLOT(onNamesReceived(QDBusPendingCallWatcher*)));
	}
	
	QMap<QString, Player::Info> MprisPlayerFactory::players()
	{
		return m_knownPlayers;
	}
	
	QObject *MprisPlayerFactory::player(const QString &id)
	{
		if (m_knownPlayers.contains(id))
			return new MprisPlayer(id);
		else
			return 0;
	}

	void MprisPlayerFactory::onServiceRegistered(const QString &service)
	{
		if (!service.startsWith(QLatin1String("org.mpris.")))
			return;
		ensureServiceInfo(service, Name);
		ensureServiceInfo(service, DesktopId);
	}

	void MprisPlayerFactory::onServiceUnregistered(const QString &service)
	{
		if (!service.startsWith(QLatin1String("org.mpris.")))
			return;
		m_knownPlayers.remove(service);
		PlayerEvent ev(service, PlayerEvent::Unavailable);
		qApp->sendEvent(this, &ev);
	}
	
	void MprisPlayerFactory::onIdentityReceived(QDBusPendingCallWatcher *watcher)
	{
		watcher->deleteLater();
		QString service = watcher->property("service").toString();
		QString identity;
		if (service.startsWith(QLatin1String("org.mpris.MediaPlayer2."))) {
			QDBusVariant variant = watcher->reply().arguments().at(0).value<QDBusVariant>();
			identity = variant.variant().toString();
		} else {
			identity = watcher->reply().arguments().at(0).toString();
		}
		m_knownPlayers[service].name = identity;
		PlayerEvent ev(service, PlayerEvent::Available);
		qApp->sendEvent(this, &ev);
	}
	
	void MprisPlayerFactory::onDesktopNameReceived(QDBusPendingCallWatcher *watcher)
	{
		watcher->deleteLater();
		QString service = watcher->property("service").toString();
		QDBusVariant variant = watcher->reply().arguments().at(0).value<QDBusVariant>();
		QString desktopId = variant.variant().toString();
		desktopId += QLatin1String(".desktop");
		QDir dir(QLatin1String("/usr/share/applications"));
		QStringList files = dir.entryList(QStringList(desktopId), QDir::Files);
		if (files.isEmpty()) {
			foreach (const QString &dirName, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
				if (!dir.cd(dirName))
					continue;
				files = dir.entryList(QStringList(desktopId), QDir::Files);
				if (!files.isEmpty())
					break;
				dir.cdUp();
			}
		}
		if (files.isEmpty())
			return;
		QSettings desktopEntry(dir.absoluteFilePath(files.first()), QSettings::IniFormat);
		QString iconName = desktopEntry.value(QLatin1String("Desktop Entry/Icon")).toString();
		QIcon icon;
		if (iconName.isEmpty())
			return;
		else if (QFileInfo(iconName).isAbsolute())
			icon = QIcon(iconName);
		else
			icon = Icon(iconName);
		m_knownPlayers[service].icon = icon;
	}
	
	void MprisPlayerFactory::onNamesReceived(QDBusPendingCallWatcher *watcher)
	{
		watcher->deleteLater();
		QDBusPendingReply<QStringList> call = *watcher;
		foreach (const QString &service, call.argumentAt<0>()) {
			if (service.startsWith(QLatin1String("org.mpris."))) {
				ensureServiceInfo(service, Name);
				ensureServiceInfo(service, DesktopId);
			}
		}
	}
	
	void MprisPlayerFactory::ensureServiceInfo(const QString &service, InfoType type)
	{
		QDBusMessage msg;
		if (service.startsWith(QLatin1String("org.mpris.MediaPlayer2."))) {
			msg = QDBusMessage::createMethodCall(service,
												 QLatin1String("/org/mpris/MediaPlayer2"),
												 QLatin1String("org.freedesktop.DBus.Properties"),
												 QLatin1String("Get"));
			msg.setArguments(QVariantList()
							 << QLatin1String("org.mpris.MediaPlayer2")
							 << QLatin1String(type == Name ? "Identity" : "DesktopEntry"));
		} else {
			if (type != Name)
				return;
			msg = QDBusMessage::createMethodCall(service,
												 QLatin1String("/"),
												 QLatin1String("org.freedesktop.MediaPlayer"),
												 QLatin1String("Identity"));
		}
		QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
		QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
		watcher->setProperty("service", service);
		connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
				this, type == Name
				? SLOT(onIdentityReceived(QDBusPendingCallWatcher*)) 
					: SLOT(onDesktopNameReceived(QDBusPendingCallWatcher*)));
	}
}
}

