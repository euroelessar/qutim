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
	addAuthor(QLatin1String("euroelessar"));
//	qDebug() << "asking for accounts";
//	AccountManagerPtr accManager = AccountManager::create();
//	qDebug() << accManager->allAccountPaths();
//	qDebug() << accManager->supportedAccountProperties();
//	foreach(Tp::AccountPtr account, accManager->allAccounts())
//	{
//		qDebug() << account->protocol() << account->displayName() << account->cmName() << account->parameters();
//	}
//	qDebug() << "after asking";
//	foreach (ConnectionManagerPtr connManager, listConnectionManagers()) {
//		QEventLoop *loop = new QEventLoop;
//		connect(connManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)), loop, SLOT(quit()));
//		loop->exec();
//		delete loop;
//		qDebug() << connManager->busName();
//		foreach (ProtocolInfo *protocol, connManager->protocols()) {
//			qDebug() << "astral" << protocol->cmName() << protocol->name();
//			addExtension(protocol->name() + " (" + protocol->cmName() + ")",
//						 QT_TRANSLATE_NOOP("Plugin", "Plugin support by Telepathy"),
//						 new AstralProtocolGenerator(connManager, protocol));
//		}
//	}
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

