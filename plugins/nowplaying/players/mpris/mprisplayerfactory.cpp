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
#include <qutim/debug.h>

namespace qutim_sdk_0_3 {

namespace nowplaying {
	
	MprisPlayerFactory::MprisPlayerFactory()
	{
		QDBusConnection conn = QDBusConnection::sessionBus();
		QDBusConnectionInterface *interface = conn.interface();
		// It's needed to avoid warning in QDBusConnectionInterface::connectNotify
		conn.connect(interface->service(), interface->path(), interface->interface(),
					 QLatin1String("NameOwnerChanged"),
					 this, SLOT(onServiceOwnerChanged(QString,QString,QString)));
		QDBusPendingCall call = interface->asyncCall(QLatin1String("ListNames"));
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

	void MprisPlayerFactory::onServiceOwnerChanged(const QString &service, const QString &from, const QString &to)
	{
		if (!service.startsWith(QLatin1String("org.mpris.")))
			return;
		if (from.isEmpty() && !to.isEmpty())
			onServiceRegistered(service);
		else if (!from.isEmpty() && to.isEmpty())
			onServiceUnregistered(service);
	}

	void MprisPlayerFactory::onServiceRegistered(const QString &service)
	{
		ensureServiceInfo(service);
	}

	void MprisPlayerFactory::onServiceUnregistered(const QString &service)
	{
		m_knownPlayers.remove(service);
		PlayerEvent ev(service, PlayerEvent::Unavailable);
		qApp->sendEvent(this, &ev);
	}

	void MprisPlayerFactory::onServiceInfoReceived(QDBusPendingCallWatcher *watcher)
	{
		watcher->deleteLater();
		QString service = watcher->property("service").toString();
		if (service.startsWith(QLatin1String("org.mpris.MediaPlayer2."))) {
			QDBusArgument argument = watcher->reply().arguments().at(0).value<QDBusArgument>();
			QVariantMap map;
			argument >> map;
			QString identity = map.value(QLatin1String("Identity")).toString();
			QString desktopId = map.value(QLatin1String("DesktopEntry")).toString();
			onIdentityReceived(service, identity);
			onDesktopNameReceived(service, desktopId);
		} else {
			QString identity = watcher->reply().arguments().at(0).toString();
			onIdentityReceived(service, identity);
		}
	}
	
	void MprisPlayerFactory::onIdentityReceived(const QString &service, const QString &identity)
	{
		m_knownPlayers[service].name = identity;
		PlayerEvent ev(service, PlayerEvent::Available);
		qApp->sendEvent(this, &ev);
	}
	
	void MprisPlayerFactory::onDesktopNameReceived(const QString &service, const QString &desktopId)
	{
		QString desktopFile = desktopId + QLatin1String(".desktop");
		QDir dir(QLatin1String("/usr/share/applications"));
		QStringList files = dir.entryList(QStringList(desktopFile), QDir::Files);
		if (files.isEmpty()) {
			foreach (const QString &dirName, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
				if (!dir.cd(dirName))
					continue;
				files = dir.entryList(QStringList(desktopFile), QDir::Files);
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
			if (service.startsWith(QLatin1String("org.mpris.")))
				ensureServiceInfo(service);
		}
	}
	
	void MprisPlayerFactory::ensureServiceInfo(const QString &service)
	{
		QDBusMessage msg;
		if (service.startsWith(QLatin1String("org.mpris.MediaPlayer2."))) {
			msg = QDBusMessage::createMethodCall(service,
												 QLatin1String("/org/mpris/MediaPlayer2"),
												 QLatin1String("org.freedesktop.DBus.Properties"),
												 QLatin1String("GetAll"));
			msg.setArguments(QVariantList() << QLatin1String("org.mpris.MediaPlayer2"));
		} else {
			msg = QDBusMessage::createMethodCall(service,
												 QLatin1String("/"),
												 QLatin1String("org.freedesktop.MediaPlayer"),
												 QLatin1String("Identity"));
		}
		QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
		QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
		watcher->setProperty("service", service);
		connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
				this, SLOT(onServiceInfoReceived(QDBusPendingCallWatcher*)));
	}
}
}

