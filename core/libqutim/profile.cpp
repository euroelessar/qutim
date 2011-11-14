/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "profile.h"
#include <QCoreApplication>
#include <QDir>
#include <QCryptographicHash>
#include "systeminfo.h"
#include "extensioninfo.h"
#include "config.h"
#include "jsonfile.h"
#include "objectgenerator.h"
#include "cryptoservice.h"

namespace qutim_sdk_0_3
{
LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs(); 
LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();

class ProfilePrivate
{
public:
};

Profile::Profile()
{
}

Profile::~Profile()
{
}

Profile *Profile::instance()
{
	static QScopedPointer<Profile> self(new Profile);
	return self.data();
}

QVariantMap Profile::data()
{
	QFileInfo profilesInfo(configPath());
	if (!profilesInfo.exists() || !profilesInfo.isFile()) {
		return QVariantMap();
	} else {
		JsonFile file(profilesInfo.absoluteFilePath());
		QVariant var;
		file.load(var);
		return var.toMap();
	}
}

QString Profile::configPath()
{
	QDir dir = qApp->applicationDirPath();
	if (!dir.exists("profiles.json") && (!dir.exists("profiles") || !dir.cd("profiles"))) {
#if defined(Q_OS_WIN)
		dir = QString::fromLocal8Bit(qgetenv("APPDATA"));
#elif defined(Q_OS_MAC)
		dir = QDir::home().absoluteFilePath("Library/Application Support");
#elif defined(Q_OS_UNIX)
		dir = QDir::home().absoluteFilePath(".config");
#else
# Undefined OS
#endif
		dir.mkpath("qutim/profiles");
		dir.cd("qutim/profiles");
	}
	return dir.filePath("profiles.json");
}

bool Profile::acceptData(const QVariantMap &data, const QString &password, QString *errors)
{
	QString tmp;
	if (!errors)
		errors = &tmp;
	Config config(data);
	QString crypto = config.value("crypto", QString());
	GeneratorList gens = ObjectGenerator::module<CryptoService>();
	CryptoService *service = 0;
	foreach (const ObjectGenerator *gen, gens) {
		if (QLatin1String(gen->metaObject()->className()) == crypto) {
			service = gen->generate<CryptoService>();
			break;
		}
	}

	QString configDir = config.value("configDir", QString());
	QFile file(configDir + "/profilehash");
	if (service && file.open(QIODevice::ReadOnly)) {
		service->setPassword(password, QVariant());
		QByteArray data = service->decrypt(file.readAll()).toByteArray();
		QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8()
														   + "5667dd05fbe97bb238711a3af63",
														   QCryptographicHash::Sha1);
		QDataStream in(data);
		QString id;
		QByteArray hash;
		QByteArray cryptoCheck;
		in >> id >> hash >> cryptoCheck;
		if (passwordHash != hash)
			*errors += tr("Password is mismatched.") + '\n';
		if (QLatin1String(cryptoCheck) != crypto)
			*errors += tr("Crypto service is unknown.") + '\n';
		if (id != config.value("id", QString()))
			*errors += tr("Wrong profile id.") + '\n';
		if (!errors->isEmpty()) {
			errors->chop(1);
			emit error(*errors);
			delete service;
			return false;
		}

		QVector<QDir> &systemDirs = *system_info_dirs();
		if (config.value("portable", false)) {
			QDir dir = qApp->applicationDirPath();
			systemDirs[SystemInfo::ConfigDir] = dir.absoluteFilePath(config.value("configDir", QString()));
			systemDirs[SystemInfo::HistoryDir] = dir.absoluteFilePath(config.value("historyDir", QString()));
			systemDirs[SystemInfo::ShareDir] = dir.absoluteFilePath(config.value("shareDir", QString()));
		} else {
			systemDirs[SystemInfo::ConfigDir] = QDir::cleanPath(config.value("configDir", QString()));
			systemDirs[SystemInfo::HistoryDir] = QDir::cleanPath(config.value("historyDir", QString()));
			systemDirs[SystemInfo::ShareDir] = QDir::cleanPath(config.value("shareDir", QString()));
		}

		QString configName = config.value("config", QString());
		QList<ConfigBackend*> &configBackends = get_config_backends();
		foreach (const ObjectGenerator *gen, ObjectGenerator::module<ConfigBackend>()) {
			const ExtensionInfo info = gen->info();
			ConfigBackend *backend = info.generator()->generate<ConfigBackend>();
			if (configName == QLatin1String(backend->metaObject()->className()))
				configBackends.prepend(backend);
			else
				configBackends.append(backend);
		}

		return true;
	} else {
		*errors = tr("Can't open file with hash");
		emit error(*errors);
		delete service;
		return false;
	}
}
}
