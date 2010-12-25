#include "dbusbackend.h"
#include <QTextDocument>
#include <QVariant>
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <qutim/config.h>
#include <QBuffer>
#include <qutim/emoticons.h>

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

	QVariantMap hints;
	if (!icon.isNull()) {
		DBusNotifyImageData imageData = { QPixmap(icon) };
		hints["image_data"] = qVariantFromValue(imageData);
	}
	hints["desktop-entry"] = QLatin1String("qutim");
	hints["sound-file"] = Sound::theme().path(type);
	int id = 0;

	icon = QLatin1String("qutim");

	QDBusPendingReply<uint> reply = interface->Notify(
			QCoreApplication::applicationName(),
			id,
			icon, //QString(), //QLatin1String("qutim"),
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

void DBusBackend::loadSettings()
{
	Config behavior = Config("behavior").group("notifications/popups");
	m_showFlags = behavior.value("showFlags", 0xfffffff &~ Notifications::MessageSend);
}
