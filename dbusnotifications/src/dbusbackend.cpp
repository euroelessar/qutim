#include "dbusbackend.h"
#include <QTextDocument>
#include <QVariant>
#include <libqutim/message.h>
#include <libqutim/buddy.h>
#include <libqutim/debug.h>
#include <QBuffer>

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

using namespace qutim_sdk_0_3;

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
}

void DBusBackend::show(qutim_sdk_0_3::Notifications::Type type, QObject* sender, const QString& body,
				   const QVariant& data)
{
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

	QVariantMap hints;
	QImage image(icon);
	if (!image.isNull()) {
		hints["image_data"] = QVariant(image);
	}
	int id = 0;

	debug() << hints << icon;

	QDBusPendingReply<uint> reply = interface->Notify(
			QCoreApplication::applicationName(),
			id,
			icon,
			title,
			text,
			QStringList(),
			hints,
			5000);

	Q_UNUSED(reply);
//	QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(reply, this);
//	connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
//			SLOT(callFinished(QDBusPendingCallWatcher*)));
}

DBusBackend::~DBusBackend()
{

}

void DBusBackend::callFinished(QDBusPendingCallWatcher *watcher)
{
	watcher->deleteLater();
}
