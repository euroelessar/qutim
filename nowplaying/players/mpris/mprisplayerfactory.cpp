#include "mprisplayerfactory.h"
#include "mprisplayer.h"
#include <QDebug>

namespace qutim_sdk_0_3 {

namespace nowplaying {
	
	MprisPlayerFactory::MprisPlayerFactory()
	{
		QDBusConnection conn = QDBusConnection::sessionBus();
		connect(conn.interface(), SIGNAL(serviceOwnerChanged(QString,QString,QString)),
				this, SLOT(onNameOwnerChanged(QString,QString,QString)));
		QDBusPendingCall call = conn.interface()->asyncCall("ListNames");
		QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
		connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
				this, SLOT(onNamesReceived(QDBusPendingCallWatcher*)));
	}
	
	QMap<QString,QString> MprisPlayerFactory::players()
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
	
	void MprisPlayerFactory::onNameOwnerChanged(const QString &service, const QString &oldName,
												const QString &newName)
	{
		if (service.startsWith(QLatin1String("org.mpris."))) {
			if (oldName.isEmpty() && !newName.isEmpty()) {
				ensureServiceName(service);
			} else if (!oldName.isEmpty() && newName.isEmpty()) {
				m_knownPlayers.remove(service);
				PlayerEvent ev(service, PlayerEvent::Unavailable);
				qApp->sendEvent(this, &ev);
			}
		}
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
		qDebug() << watcher->error().message() << watcher->reply().arguments();
		m_knownPlayers.insert(service, identity);
		PlayerEvent ev(service, PlayerEvent::Available);
		qApp->sendEvent(this, &ev);
	}
	
	void MprisPlayerFactory::onNamesReceived(QDBusPendingCallWatcher *watcher)
	{
		watcher->deleteLater();
		QDBusPendingReply<QStringList> call = *watcher;
		foreach (const QString &service, call.argumentAt<0>()) {
			if (service.startsWith(QLatin1String("org.mpris.")))
				ensureServiceName(service);
		}
	}
	
	void MprisPlayerFactory::ensureServiceName(const QString &service)
	{
		QDBusMessage msg;
		if (service.startsWith(QLatin1String("org.mpris.MediaPlayer2."))) {
			msg = QDBusMessage::createMethodCall(service,
												 QLatin1String("/org/mpris/MediaPlayer2"),
												 QLatin1String("org.freedesktop.DBus.Properties"),
												 QLatin1String("Get"));
			msg.setArguments(QVariantList()
							 << QLatin1String("org.mpris.MediaPlayer2")
							 << QLatin1String("Identity"));
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
				this, SLOT(onIdentityReceived(QDBusPendingCallWatcher*)));
	}
}
}
