#include "dbusbackend.h"
#include <QTextDocument>
#include <QVariant>
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/messagesession.h>
#include <QBuffer>

#ifdef Q_WS_MAEMO_5
#include <mce/mode-names.h>
#include <mce/dbus-names.h>
#endif

using namespace qutim_sdk_0_3;

QDBusArgument& operator<< (QDBusArgument& arg, const QImage& image) {
	if (image.isNull()) {
		// Sometimes this gets called with a null QImage for no obvious reason.
		arg.beginStructure();
		arg << 0 << 0 << 0 << false << 0 << 0 << QByteArray();
		arg.endStructure();
		return arg;
	}
	QImage scaled = image.scaledToHeight(100, Qt::SmoothTransformation);
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

const QDBusArgument& operator>> (const QDBusArgument& arg, QImage& image) {
	// This is needed to link but shouldn't be called.
	Q_ASSERT(0);
	return arg;
}

DBusBackend::DBusBackend() :
		interface(new org::freedesktop::Notifications(
				"org.freedesktop.Notifications",
				"/org/freedesktop/Notifications",
				QDBusConnection::sessionBus()))
{
	qDBusRegisterMetaType<QImage>();

	if (!interface->isValid()) {
		qWarning() << "Error connecting to notifications service.";
	}

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
				this, SLOT(onNotificationClosed(quint32)));

	enableVibration();
	
	loadSettings();
}

void DBusBackend::show(qutim_sdk_0_3::Notifications::Type type, QObject* sender, const QString& body,
				   const QVariant& data)
{
	if(!m_showFlags & type)
		return;
	QString text = Qt::escape(body);
	QString sender_id = sender ? sender->property("id").toString() : QString();
	QString sender_name = sender ? sender->property("name").toString() : QString();
	if(sender_name.isEmpty())
		sender_name = sender_id;
	QString title = Notifications::toString(type).arg(sender_name);

	QString icon;
	if (data.canConvert<Message>() && (type & Notifications::MessageSend & Notifications::MessageGet)) {
		const Message &msg = data.value<qutim_sdk_0_3::Message>();
		title = qutim_sdk_0_3::Notifications::toString(type).arg(msg.chatUnit()->title());
		if (const Buddy *b = qobject_cast<const Buddy*>(msg.chatUnit()))
			icon = b->avatar();
	} else if (data.canConvert<QString>()) {
		title = data.toString();
	}
	if(sender && icon.isEmpty())
		icon = sender->property("avatar").toString();

	QStringList actions;
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

	QVariantMap hints;
	QImage image(icon);
	if (!image.isNull()) {
		hints["image_data"] = QVariant(image);
	}
	int id = 0;

	icon = QLatin1String("qutim");

	QDBusPendingReply<uint> reply = interface->Notify(
			QCoreApplication::applicationName(),
			id,
			icon,
			title,
			text,
			actions,
			hints,
			5000);

	vibrate(50);
	NotificationData notification = { sender, body, data };
	m_senders.insert(reply.value(), notification);

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

void DBusBackend::loadSettings()
{
	Config behavior = Config("behavior").group("notifications/popups");
	m_showFlags = behavior.value("showFlags", 0xfffffff &~ Notifications::MessageSend);
}

void DBusBackend::onActionInvoked(quint32 id, const QString &action)
{
	NotificationData notification = m_senders.value(id);
	QObject *sender = notification.sender;
	ChatUnit *unit = qobject_cast<ChatUnit *>(sender);
	if (action == "openChat") {
		if (unit) {
			ChatUnit *metaContact = unit->metaContact();
			if (metaContact)
				unit = metaContact;
			ChatLayer::get(unit,true)->activate();
		}
	} else if (action == "ignore") {
		ChatSession *sess;
		if (unit && (sess = ChatLayer::get(unit,false))) {
			if (notification.data.canConvert<Message>())
				sess->markRead(notification.data.value<Message>().id());
		}
	} else if (action == "open") {
		if (QWidget *widget = qobject_cast<QWidget *>(sender)) {
			widget->raise();
		}
	}
}

void DBusBackend::onNotificationClosed(quint32 id)
{
	m_senders.remove(id);
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
