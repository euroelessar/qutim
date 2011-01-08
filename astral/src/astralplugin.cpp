#include "astralplugin.h"
#include <QtCore/QtDebug>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Types>
//#include <TelepathyQt4/ConnectionInterfaceAvatarsInterface>

AstralPlugin::AstralPlugin()
{
}

QList<ConnectionManagerPtr> AstralPlugin::listConnectionManagers()
{
	QList<ConnectionManagerPtr> list;
	QDBusConnection bus = QDBusConnection::sessionBus();
	QDBusInterface conn("org.freedesktop.DBus", "/", "org.freedesktop.DBus", bus, this);
	foreach (const QString &service,
			 QDBusReply<QStringList>(conn.call(QDBus::Block, "ListActivatableNames")).value()) {
		if (service.startsWith("org.freedesktop.Telepathy.ConnectionManager."))
			list << ConnectionManager::create(bus, service.section(QLatin1Char('.'), 4));
	}
	return list;
}

void AstralPlugin::init()
{
	Tp::registerTypes();
	Tp::enableDebug(true);
	Tp::enableWarnings(true);
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Astral"),
			QT_TRANSLATE_NOOP("Plugin", "Protocol support by Telepathy"),
			PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("euroelessar@gmail.com"));
//	qDebug() << "asking for accounts";
//	AccountManagerPtr accManager = AccountManager::create();
//	qDebug() << accManager->allAccountPaths();
//	qDebug() << accManager->supportedAccountProperties();
//	foreach(Tp::AccountPtr account, accManager->allAccounts())
//	{
//		qDebug() << account->protocol() << account->displayName() << account->cmName() << account->parameters();
//	}
//	qDebug() << "after asking";
	foreach (ConnectionManagerPtr connManager, listConnectionManagers()) {
		QEventLoop *loop = new QEventLoop;
		connect(connManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)), loop, SLOT(quit()));
		loop->exec();
		delete loop;
		qDebug() << connManager->busName();
		foreach (ProtocolInfo *protocol, connManager->protocols()) {
			qDebug() << "astral" << protocol->cmName() << protocol->name();
			addExtension(protocol->name() + " (" + protocol->cmName() + ")",
						 QT_TRANSLATE_NOOP("Plugin", "Plugin support by Telepathy"),
						 new AstralProtocolGenerator(connManager, protocol));
		}
	}
}

bool AstralPlugin::load()
{
	return true;
}

bool AstralPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(AstralPlugin)
