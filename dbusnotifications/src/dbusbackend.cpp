#include "dbusbackend.h"
#include <QTextDocument>
#include <QVariant>
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/messagesession.h>
#include <QBuffer>
#include <qutim/emoticons.h>
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
	
	loadSettings();
}

void DBusBackend::show(qutim_sdk_0_3::Notifications::Type type, QObject* sender, const QString& body,
				   const QVariant& data)
{
	if(!(m_showFlags & type))
		return;
	QString text = Qt::escape(body);
	QString sender_id = sender ? sender->property("id").toString() : QString();
	QString sender_name = sender ? sender->property("name").toString() : QString();
	if(sender_name.isEmpty())
		sender_name = sender_id;
	QString title = Notifications::toString(type).arg(sender_name);
	QList<QVariant> dataList;

	QString icon;
	if (data.canConvert<Message>() && (type & (Notifications::MessageSend | Notifications::MessageGet))) {
		const Message &msg = data.value<qutim_sdk_0_3::Message>();
		title = qutim_sdk_0_3::Notifications::toString(type).arg(msg.chatUnit()->title());
		if (const Buddy *b = qobject_cast<const Buddy*>(msg.chatUnit()))
			icon = b->avatar();
	} else if (data.canConvert<QString>()) {
		title = data.toString();
	}
	if(sender && icon.isEmpty())
		icon = sender->property("avatar").toString();
	if (!icon.isEmpty() && QFileInfo(icon).exists()) {
		QUrl url(icon);
		url.setScheme(QLatin1String("file"));
		icon = url.toString();
	} else {
		icon = QString();
	}
	//if (!icon.isEmpty()) {
	//	text = QLatin1String("<image alt=\"avatar\" src=\"file://") + icon + QLatin1String("\"/>");
	//}

	QStringList actions;
	if (m_capabilities.contains(QLatin1String("actions"))) {
		if (type & Notifications::MessageSend ||
			type & Notifications::MessageGet ||
			type & Notifications::Typing ||
			type & Notifications::StatusChange ||
			type & Notifications::BlockedMessage)
		{
			actions << "openChat" << tr("Open chat")
					<< "ignore" << tr("Ignore");
		} else if (qobject_cast<QWidget *>(sender)) {
			actions << "open" << tr("Open");
		}
	}

	QVariantMap hints;
	if (!icon.isNull()) {
		DBusNotifyImageData imageData = { QPixmap(icon) };
		hints["image_data"] = qVariantFromValue(imageData);
	}
	hints["desktop-entry"] = QLatin1String("qutim");
	hints["sound-file"] = Sound::theme().path(type);

	int id = m_ids.value(sender, 0);
	if (id != 0) {
		NotificationData notification = m_notifications.value(id);
		text = notification.body + "\n" + text;
		dataList = notification.data;
	}
	dataList << data;

	icon = QLatin1String("qutim");

	QDBusPendingReply<uint> reply = interface->Notify(
			QCoreApplication::applicationName(),
			id,
			icon, //QString(), //QLatin1String("qutim"),
			title,
			text,
			actions,
			hints,
			5000);

	vibrate(50);

	NotificationData notification = { sender, text, dataList };
	quint32 newId = reply.value();
	m_notifications.insert(newId, notification);
	m_ids.insert(sender, newId);
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

void DBusBackend::loadSettings()
{
	Config behavior = Config("behavior").group("notifications/popups");
	m_showFlags = behavior.value("showFlags", 0xfffffff &~ Notifications::MessageSend);
}

void DBusBackend::onActionInvoked(quint32 id, const QString &action)
{
	NotificationData notification = m_notifications.value(id);
	QObject *sender = notification.sender;
	if (action == "openChat") {
		ChatUnit *unit = qobject_cast<ChatUnit *>(sender);
		if (unit) {
			ChatUnit *metaContact = unit->metaContact();
			if (metaContact)
				unit = metaContact;
			ChatLayer::get(unit,true)->activate();
		}
	} else if (action == "ignore") {
		ignore(notification);
	} else if (action == "open") {
		if (QWidget *widget = qobject_cast<QWidget *>(sender))
			widget->raise();
	}
}

void DBusBackend::ignore(NotificationData &notification)
{
	ChatUnit *unit = qobject_cast<ChatUnit *>(notification.sender);
	ChatSession *sess;
	if (unit && (sess = ChatLayer::get(unit,false))) {
		foreach (const QVariant &data, notification.data) {
			if (data.canConvert<Message>())
				sess->markRead(data.value<Message>().id());
		}
	}
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
#endif
}

void DBusBackend::vibrate(int aTimeout)
{
#ifdef Q_WS_MAEMO_5
	mDbusInterface->call(MCE_ACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
	QTimer::singleShot(aTimeout,this,SLOT(stopVibration()));
#endif
}


void DBusBackend::stopVibration()
{
#ifdef Q_WS_MAEMO_5
	mDbusInterface->call(MCE_DEACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
#endif
}
