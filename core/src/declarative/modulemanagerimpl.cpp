/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "modulemanagerimpl.h"
#include <qutim/jsonfile.h>
#include <QVariant>
#include <QFile>
#include <qutim/protocol.h>
#include <qutim/config.h>
#include <qutim/notification.h>
#include <qutim/systemintegration.h>
#include <QDeclarativeContext>
#include <pwd.h>
#include <QTextCodec>
#include <QApplication>

#include <qutim/debug.h>
#include <qutim/systeminfo.h>
#include <qutim/profile.h>
#include <qutim/objectgenerator.h>
#include <qutim/cryptoservice.h>
#include <QDesktopServices>
#include <QTimer>
#include <QSslSocket>

namespace qutim_sdk_0_3
{
LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs(); 
LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();
}

#define CONFIG_BACKEND QLatin1String("Core::JsonConfigBackend")
#define CRYPTO_BACKEND QLatin1String("Core::NoCryptoService")

namespace Core
{
ModuleManagerImpl::ModuleManagerImpl()
{
	ModuleManager::loadPlugins();

	CryptoService *cryptoService = NULL;
	foreach (const ObjectGenerator *gen, ObjectGenerator::module<CryptoService>()) {
		if (CRYPTO_BACKEND == QLatin1String(gen->metaObject()->className())) {
			cryptoService = gen->generate<CryptoService>();
			break;
		}
	}
	QList<ConfigBackend*> &configBackends = get_config_backends();
	foreach (const ObjectGenerator *gen, ObjectGenerator::module<ConfigBackend>()) {
		const ExtensionInfo info = gen->info();
		ConfigBackend *backend = info.generator()->generate<ConfigBackend>();
		if (QLatin1String(backend->metaObject()->className()) == CONFIG_BACKEND)
			configBackends.prepend(backend);
		else
			configBackends.append(backend);
	}
	Q_ASSERT(cryptoService);
	Q_ASSERT(!configBackends.isEmpty());
	
	QVector<QDir> &dirs = *system_info_dirs();
	dirs[SystemInfo::ConfigDir] = QDir::home().absoluteFilePath(".config/qutim");
	dirs[SystemInfo::HistoryDir] = QDir::home().absoluteFilePath(".config/qutim/history");
	dirs[SystemInfo::ShareDir] = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

	Config config = Profile::instance()->config();
	if (!config.hasChildGroup("profile")) {
		QString user;
		QT_TRY {
			struct passwd *userInfo = getpwuid(getuid());
			QTextCodec *codec = QTextCodec::codecForLocale();
			user = codec->toUnicode(userInfo->pw_name);
		} QT_CATCH(...) {
		}
		config.beginGroup("profile");
		config.setValue("name", user);
		config.setValue("id", user);
		config.setValue("crypto",cryptoService->metaObject()->className());
		config.setValue("config", QLatin1String(configBackends.first()->metaObject()->className()));
		config.setValue("portable", false);
		
		config.setValue("configDir", SystemInfo::getPath(SystemInfo::ConfigDir));
		config.setValue("historyDir", SystemInfo::getPath(SystemInfo::HistoryDir));
		config.setValue("shareDir", SystemInfo::getPath(SystemInfo::ShareDir));
		config.endGroup();
	}

	QTimer::singleShot(0, this, SLOT(initExtensions()));
}

ExtensionInfoList ModuleManagerImpl::coreExtensions() const
{
	return ExtensionInfoList();
}

void ModuleManagerImpl::initExtensions()
{
	QString path = SystemInfo::getPath(SystemInfo::SystemShareDir);
	path += QLatin1String("/ca-certs/*.pem");
	QSslSocket::addDefaultCaCertificates(path, QSsl::Pem, QRegExp::Wildcard);
	
	ModuleManager::initExtensions();

	NotificationRequest request(Notification::AppStartup);
	request.send();
}
}
