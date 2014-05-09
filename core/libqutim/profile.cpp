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
#include "debug.h"

namespace qutim_sdk_0_3
{
LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs(); 
LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();

class SimpleJsonConfigBackend : public ConfigBackend
{
public:
	virtual QVariant load(const QString &filePath)
	{
		QVariant var;
		JsonFile file(filePath);
		file.load(var);
		return var;
	}

	virtual void save(const QString &filePath, const QVariant &entry)
	{
		JsonFile file(filePath);
		file.save(entry);
	}
};

Q_GLOBAL_STATIC(SimpleJsonConfigBackend, configBackend)

class ProfilePrivate
{
public:
	static Config profileConfig(bool *ok);
};

Config ProfilePrivate::profileConfig(bool *ok)
{
	bool tmp = true;
	if (!ok) ok = &tmp;

	Config config = Profile::instance()->config();
#ifdef QUTIM_SINGLE_PROFILE
	bool singleProfile = true;
#else
	bool singleProfile = false;
#endif
	singleProfile = config.value("singleProfile", singleProfile);
	if (singleProfile) {
		config.beginGroup(QLatin1String("profile"));
		*ok = true;
		return config;
	}
	QString current = config.value(QLatin1String("current"), QString());
	int arraySize = config.beginArray(QLatin1String("list"));
	for (int i = 0; i < arraySize; ++i) {
		config.setArrayIndex(i);
		if (config.value(QLatin1String("id"), QString()) == current) {
			*ok = true;
			return config;
		}
	}
	*ok = false;
	return config;
}

Profile::Profile()
{
	_customProfilePath = "";
}

Profile::~Profile()
{
}

Profile *Profile::instance()
{
	static QScopedPointer<Profile> self(new Profile);
	return self.data();
}

Config Profile::config()
{
	return Config(configPath(), configBackend());
}

QVariant Profile::value(const QString &key) const
{
	bool ok = false;
	Config cfg = ProfilePrivate::profileConfig(&ok);
	Q_ASSERT(ok);
	if (!ok)
		return QVariant();
	return cfg.value(key);
}

void Profile::setValue(const QString &key, const QVariant &value)
{
	bool ok = false;
	Config cfg = ProfilePrivate::profileConfig(&ok);
	Q_ASSERT(ok);
	if (!ok)
		return;
	cfg.setValue(key, value);
}

QString Profile::configPath()
{
	return Profile::configPath(0);
}

QString Profile::configPath(bool *isSystem)
{
	bool tmpSystem = true;
	if (!isSystem)
		isSystem = &tmpSystem;
	*isSystem = false;

	QDir dir = qApp->applicationDirPath();
	if (dir.exists("profiles.json") || (dir.exists("profiles") && dir.cd("profiles")))
		return dir.filePath("profiles.json");

	QString systemProfiles;
	dir = SystemInfo::getDir(SystemInfo::SystemConfigDir);
	if (dir.exists("profiles.json"))
		systemProfiles = dir.filePath("profiles.json");

#if defined(Q_OS_WIN)
	dir = QString::fromLocal8Bit(qgetenv("APPDATA"));
#elif defined(Q_OS_MAC)
	dir = QDir::home().absoluteFilePath("Library/Application Support");
#elif defined(Q_OS_UNIX)
	dir = QDir::home().absoluteFilePath(".config");
#else
# Undefined OS
#endif

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
	dir.mkpath("qutIM/profiles");
	dir.cd("qutIM/profiles");
#else
	dir.mkpath("qutim/profiles");
	dir.cd("qutim/profiles");
#endif
	if (!_customProfilePath.isEmpty()) {
		dir = QDir(_customProfilePath);
		if(!dir.exists())
			if(!QDir::root().mkpath(_customProfilePath))
				qFatal("No such profile directory");
		return dir.filePath("profiles.json");
	} else if (!systemProfiles.isEmpty() && !dir.exists("profiles.json")) {
		*isSystem = true;
		return systemProfiles;
	} else {
		return dir.filePath("profiles.json");
	}
}

bool Profile::acceptData(const QVariantMap &profilesData, const QString &password, QString *error)
{
	return acceptData(profilesData, password, true, error);
}

static QString replaceEnvironmentVariables(const QString &path)
{
	QString cleanedPath = QDir::cleanPath(path);
	QRegExp regexp(QLatin1String("%(\\w+)%"));
	Q_ASSERT(regexp.isValid());
	int pos = 0;
   
    while ((pos = regexp.indexIn(cleanedPath, pos)) != -1) {
        QByteArray variable = regexp.cap(1).toLocal8Bit();
		QByteArray localEncodedVariable = qgetenv(variable);
		QString data = QString::fromLocal8Bit(localEncodedVariable.constData(), localEncodedVariable.size());
		if (data.isEmpty()) {
			pos += regexp.matchedLength();
		} else {
			cleanedPath.replace(pos, regexp.matchedLength(), data);
			pos += data.length();
		}
    }
	return cleanedPath;
}

bool Profile::acceptData(const QVariantMap &data, const QString &password, bool checkHash, QString *errors)
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
	if (config.value("portable", false)) {
		QDir dir = qApp->applicationDirPath();
		configDir = dir.absoluteFilePath(configDir);
	}
	QFile file(configDir + "/profilehash");
	if (service && (!checkHash || file.open(QIODevice::ReadOnly))) {
		service->setPassword(password, QVariant());
		
		if (checkHash) {
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
		}

		QVector<QDir> &systemDirs = *system_info_dirs();
		if (config.value("portable", false)) {
			QDir dir = qApp->applicationDirPath();
			systemDirs[SystemInfo::ConfigDir] = dir.absoluteFilePath(config.value("configDir", QString()));
			systemDirs[SystemInfo::HistoryDir] = dir.absoluteFilePath(config.value("historyDir", QString()));
			systemDirs[SystemInfo::ShareDir] = dir.absoluteFilePath(config.value("shareDir", QString()));
		} else {
			systemDirs[SystemInfo::ConfigDir] = replaceEnvironmentVariables(config.value("configDir", QString()));
			systemDirs[SystemInfo::HistoryDir] = replaceEnvironmentVariables(config.value("historyDir", QString()));
			systemDirs[SystemInfo::ShareDir] = replaceEnvironmentVariables(config.value("shareDir", QString()));
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
		debugClearConfig();

		return true;
	} else {
		*errors = tr("Can't open file with hash");
		emit error(*errors);
		delete service;
		return false;
	}
}

void Profile::setCustomProfilePath(const QString &path)
{
	_customProfilePath = path;
}

QString Profile::getCustomProfilePath()
{
	return _customProfilePath;
}

bool Profile::isCustomProfilePath()
{
	return !_customProfilePath.isEmpty();
}

} // namespace
