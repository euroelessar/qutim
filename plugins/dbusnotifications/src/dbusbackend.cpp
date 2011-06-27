#include "dbusbackend.h"
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/messagesession.h>
#include <qutim/emoticons.h>
#include <QTextDocument>
#include <QVariant>
#include <QBuffer>
#include <QDBusPendingReply>

#ifdef Q_WS_MAEMO_5
#include <mce/mode-names.h>
#include <mce/dbus-names.h>
#endif

using namespace qutim_sdk_0_3;

struct DBusNotifyImageData
{
	QPixmap image;
};

Q_DECLARE_METATYPE(DBusNotifyImageData)

QDBusArgument& operator<< (QDBusArgument& arg, const DBusNotifyImageData &data) {
	if (data.image.isNull()) {
		// Sometimes this gets called with a null QImage for no obvious reason.
		// - There is one reason: Qt calls this method at first time to research it's structure
		arg.beginStructure();
		arg << 0 << 0 << 0 << false << 0 << 0 << QByteArray();
		arg.endStructure();
		return arg;
	}
	QImage scaled = data.image.scaledToHeight(qMin(100, qMin(data.image.height(), data.image.width())),
											  Qt::SmoothTransformation).toImage();
	QImage i = scaled.convertToFormat(QImage::Format_ARGB32).rgbSwapped();
	arg.beginStructure();
	arg << i.width();
	arg << i.height();
	arg << i.bytesPerLine();
	arg << i.hasAlphaChannel();
	int channels = i.isGrayscale() ? 1 : (i.hasAlphaChannel() ? 4 : 3);
	arg << i.depth() / channels;
	arg << channels;
	arg << QByteArray(reinterpret_cast<const char*>(i.bits()), i.numBytes());
	arg.endStructure();
	return arg;
}

const QDBusArgument& operator>> (const QDBusArgument& arg, DBusNotifyImageData &data) {
	Q_ASSERT(!"This shouldn't ever be happened");
	Q_UNUSED(data);
	return arg;
}

DBusBackend::DBusBackend() :
	NotificationBackend("Popup"),
	interface(new org::freedesktop::Notifications(
			"org.freedesktop.Notifications",
			"/org/freedesktop/Notifications",
			QDBusConnection::sessionBus()))
{
	qDBusRegisterMetaType<DBusNotifyImageData>();

	if (!interface->isValid()) {
		qWarning() << "Error connecting to notifications service.";
	}
	QDBusMessage message = QDBusMessage::createMethodCall(
	            QLatin1String("org.freedesktop.Notifications"),
	            QLatin1String("/org/freedesktop/Notifications"),
	            QLatin1String("org.freedesktop.Notifications"),
	            QLatin1String("GetCapabilities"));
	QDBusPendingReply<QStringList> call = QDBusConnection::sessionBus().asyncCall(message);
	QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
	connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
	        this, SLOT(capabilitiesCallFinished(QDBusPendingCallWatcher*)));

	QDBusConnection::sessionBus().connect(
				QString(),
				"/org/freedesktop/Notifications",
				"org.freedesktop.Notifications",
				"ActionInvoked",
				this, SLOT(onActionInvoked(quint32,QString)));

	QDBusConnection::sessionBus().connect(
				QString(),
				"/org/freedesktop/Notifications",
				"org.freedesktop.Notifications",
				"NotificationClosed",
				this, SLOT(onNotificationClosed(quint32,quint32)));

	enableVibration();
}

void DBusBackend::handleNotification(qutim_sdk_0_3::Notification *notification)
{
	ref(notification);
	NotificationRequest request = notification->request();
	NotificationData data;

	QString text = Qt::escape(request.text());

	QVariantMap hints;
	if (!request.image().isNull()) {
		DBusNotifyImageData imageData = { request.image() };
		hints["image_data"] = qVariantFromValue(imageData);
	}
	hints["desktop-entry"] = QLatin1String("qutim");
	hints["sound-file"] = Sound::theme().path(request.type());

	int id = m_ids.value(request.object(), 0);
	if (id) {
		data = m_notifications.value(id);
#ifndef QUTIM_MOBILE_UI
		data.body = data.body + "\n" + text;
#else
		data.body = text + "\n" + data.body;
#endif
	} else {
		data.sender = request.object();
		data.body = text;
	}

	data.notifications << notification;

	QStringList actions;
	if (m_capabilities.contains(QLatin1String("actions"))) {
		foreach (const NotificationAction &action, request.actions()) {
			QString name = action.title().original();
			data.actions.insert(name, action);
			actions << name << action.title();
		}
	}

#ifndef QUTIM_MOBILE_UI
	int timeout = 5000;
#else
	int timeout = 500000;
#endif

	QDBusPendingReply<uint> reply = interface->Notify(
				QCoreApplication::applicationName(),
				id,
				QLatin1String("qutim"),
				request.title(),
				text,
				actions,
				hints,
				timeout);

	vibrate(50);

	int newId = reply.value();
	m_notifications.insert(newId, data);
	m_ids.insert(request.object(), newId);
	if (id != newId)
		m_notifications.remove(id);

	Q_UNUSED(reply);
	//	QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(reply, this);
	//	connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
	//			SLOT(callFinished(QDBusPendingCallWatcher*)));
}


DBusBackend::~DBusBackend()
{
#ifdef Q_WS_MAEMO_5
	//mDbusInterface->call(MCE_DISABLE_VIBRATOR);
	mDbusInterface->call(MCE_DEACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
#endif
}

void DBusBackend::callFinished(QDBusPendingCallWatcher *watcher)
{
	watcher->deleteLater();
}

void DBusBackend::capabilitiesCallFinished(QDBusPendingCallWatcher* watcher)
{
	watcher->deleteLater();
	QDBusPendingReply<QStringList> reply = *watcher;
	m_capabilities = QSet<QString>::fromList(reply.argumentAt<0>());
}

void DBusBackend::onActionInvoked(quint32 id, const QString &name)
{
	NotificationData data = m_notifications.value(id);
	foreach (const NotificationAction &action, data.actions.values(name))
		action.trigger();
}

inline void DBusBackend::ignore(NotificationData &data)
{
	Q_UNUSED(data);
	foreach (const QPointer<Notification> &notification, data.notifications)
		if (notification)
			notification->ignore();
}

void DBusBackend::onNotificationClosed(quint32 id, quint32 reason)
{
	/*
	  The reasons:
	  1 - The notification expired.
	  2 - The notification was dismissed by the user.
	  3 - The notification was closed by a call to CloseNotification.
	  4 - Undefined/reserved reasons.
	 */

	QHash<quint32, NotificationData>::iterator itr = m_notifications.find(id);
	if (itr != m_notifications.end()) {
		if (reason == 2)
			ignore(*itr);
		m_ids.remove(itr->sender);
		foreach (QPointer<Notification> notification, itr->notifications)
			deref(notification.data());
		m_notifications.erase(itr);
	}
}

void DBusBackend::enableVibration()
{
#ifdef Q_WS_MAEMO_5
	mDbusInterface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH,
										MCE_REQUEST_IF, QDBusConnection::systemBus(),
										this);
	mDbusInterface->call(MCE_ENABLE_VIBRATOR);
	mDbusInterface->call(MCE_ENABLE_LED);
	display_off=false;
	QDBusConnection::systemBus().connect(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
										 MCE_DISPLAY_SIG, this,SLOT(displayStateChanged(const QDBusMessage &)));
	mDbusInterface->callWithCallback(MCE_DISPLAY_STATUS_GET, QList<QVariant>(), this, SLOT(setDisplayState(const QString &)));
#endif
}

void DBusBackend::vibrate(int aTimeout)
{
#ifdef Q_WS_MAEMO_5
    if (display_off)
    {
		mDbusInterface->call(MCE_ACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
		QTimer::singleShot(aTimeout,this,SLOT(stopVibration()));
		mDbusInterface->call(MCE_ACTIVATE_LED_PATTERN, "PatternCommunicationIM");
    }
#else
	Q_UNUSED(aTimeout);
#endif
}


void DBusBackend::stopVibration()
{
#ifdef Q_WS_MAEMO_5
	mDbusInterface->call(MCE_DEACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
#endif
}

void DBusBackend::displayStateChanged(const QDBusMessage &message)
{
#ifdef Q_WS_MAEMO_5
	QString state = message.arguments().at(0).toString();
	setDisplayState(state);
#else
	Q_UNUSED(message);
#endif
}

void DBusBackend::setDisplayState(const QString &state)
{
#ifdef Q_WS_MAEMO_5
	if (!state.isEmpty()) {
		if (state == MCE_DISPLAY_ON_STRING)
		{
			display_off=false;
		}
		else if (state == MCE_DISPLAY_OFF_STRING)
		{
			display_off=true;
		}
	}
#else
	Q_UNUSED(state);
#endif
}
