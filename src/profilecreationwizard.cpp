#include "profilecreationwizard.h"
#include "libqutim/profilecreatorpage.h"
#include "libqutim/objectgenerator.h"
#include "libqutim/systeminfo.h"
#include "profilecreationpage.h"
#include "libqutim/jsonfile.h"
#include "libqutim/configbase_p.h"
#include "libqutim/game/config.h"
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#ifdef Q_OS_UNIX
# include <pwd.h>
#endif

namespace qutim_sdk_0_3
{ LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs(); }

namespace Core
{
inline bool creatorsLessThan(ProfileCreatorPage *a, ProfileCreatorPage *b)
{
	return a->priority() > b->priority();
}

ProfileCreationWizard::ProfileCreationWizard(ModuleManager *parent,
											 const QString &id, const QString &password,
											 bool singleProfile)
{
	m_manager = parent;
	m_singleProfile = singleProfile;
	
	QDir tmpDir = QDir::temp();
	QString path = "qutim/profile";
	if (tmpDir.exists(path) || !tmpDir.mkpath(path) || !tmpDir.cd(path)) {
		for (int i = 0;; i++) {
			QString tmpPath = path + QString::number(i);
			if (!tmpDir.exists(tmpPath)) {
				path = tmpPath;
				if (tmpDir.mkpath(path) && tmpDir.cd(path)) {
					break;
				} else {
					qFatal("Can't access or create directory '%s'",
						   qPrintable(tmpDir.absoluteFilePath(path)));
				}
			}
		}
	}
	QVector<QDir> &systemDirs = *system_info_dirs();
	systemDirs[SystemInfo::ConfigDir] = tmpDir.absoluteFilePath("config");
	systemDirs[SystemInfo::HistoryDir] = tmpDir.absoluteFilePath("history");
	systemDirs[SystemInfo::ShareDir] = tmpDir.absoluteFilePath("share");
	for (int i = SystemInfo::ConfigDir; i <= SystemInfo::ShareDir; i++)
		tmpDir.mkdir(systemDirs[i].dirName());
	qDebug() << Q_FUNC_INFO << SystemInfo::getPath(SystemInfo::ConfigDir);
	
	addPage(new ProfileCreationPage(password, singleProfile, this));
	QString realId;
	QString realName;
#ifdef Q_OS_UNIX
	if (id.isEmpty()) {
		QT_TRY {
			struct passwd *userInfo = getpwuid(getuid());
			QTextCodec *codec = QTextCodec::codecForLocale();
			realId = codec->toUnicode(userInfo->pw_name);
			realName = codec->toUnicode(userInfo->pw_gecos).section(',', 0, 0);
		} QT_CATCH(...) {
		}
	} else {
		realId = id;
		realName = id;
	}
#endif
	setField("id", realId);
	setField("name", realName);
	QList<ProfileCreatorPage *> creators;
	foreach (const ObjectGenerator *gen, moduleGenerators<ProfileCreatorPage>()) {
		ProfileCreatorPage *creator = gen->generate<ProfileCreatorPage>();
		creator->setParent(this);
		creators << creator;
	}
	qSort(creators.begin(), creators.end(), creatorsLessThan);
	foreach (ProfileCreatorPage *creator, creators) {
		foreach (QWizardPage *page, creator->pages(this)) {
			addPage(page);
		}
	}
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
}

void ProfileCreationWizard::done(int result)
{
//	qDebug() << Q_FUNC_INFO << result << (*system_info_dirs());
//	qDebug() << Q_FUNC_INFO << SystemInfo::getPath(SystemInfo::ConfigDir);
	if (result == Accepted) {
//		QVector<QDir> &systemDirs = *system_info_dirs();
//		systemDirs[SystemInfo::ConfigDir] = QDir::cleanPath(field("configDir").toString());
//		systemDirs[SystemInfo::HistoryDir] = QDir::cleanPath(field("historyDir").toString());
//		systemDirs[SystemInfo::ShareDir] = QDir::cleanPath(field("dataDir").toString());
		QDir dir;
		if (field("portable").toBool()) {
			dir = qApp->applicationDirPath();
		} else {
#if defined(Q_OS_WIN)
			dir = QString::fromLocal8Bit(qgetenv("APPDATA")) + "/qutim";
#elif defined(Q_OS_MAC)
			dir = QDir::homePath() + "/Library/Application Support/qutIM";
#elif defined(Q_OS_UNIX)
			dir = QDir::home().absoluteFilePath(".config/qutim");
#else
# Undefined OS
#endif
		}
		ProfileCreationPage *page = findChild<ProfileCreationPage*>();
		QVariantMap map;
		JsonFile file;
		file.setFileName(dir.absoluteFilePath("profiles/profiles.json"));
		QVariant var;
		if (file.load(var))
			map = var.toMap();
		{
			Game::Config config(&map);
			if (m_singleProfile) {
				config.beginGroup("profile");
			} else {
				int size = config.beginArray("list");
				config.setArrayIndex(size);
			}
			config.setValue("name", field("name"));
			config.setValue("id", field("id"));
			config.setValue("crypto", QLatin1String(page->cryptoName()));
			config.setValue("config", QLatin1String(ConfigPrivate::config_backends.first()
													.second->metaObject()->className()));
			config.setValue("portable", field("portable"));
			if (field("portable").toBool()) {
				QDir app = qApp->applicationDirPath();
				config.setValue("configDir", app.relativeFilePath(SystemInfo::getPath(SystemInfo::ConfigDir)));
				config.setValue("historyDir", app.relativeFilePath(SystemInfo::getPath(SystemInfo::HistoryDir)));
				config.setValue("shareDir", app.relativeFilePath(SystemInfo::getPath(SystemInfo::ShareDir)));
			} else {
				config.setValue("configDir", SystemInfo::getPath(SystemInfo::ConfigDir));
				config.setValue("historyDir", SystemInfo::getPath(SystemInfo::HistoryDir));
				config.setValue("shareDir", SystemInfo::getPath(SystemInfo::ShareDir));
			}
			if (m_singleProfile) {
				config.endGroup();
			} else {
				config.endArray();
				if (config.value("current").isNull())
					config.setValue("current", field("id"));
			}
		}
		if (!file.save(map)) {
//			QMessageBox::critical(this, tr(""))
			qWarning("Can not open file '%s' for writing", 
					 qPrintable(dir.absoluteFilePath("profiles/profiles.json")));
			QTimer::singleShot(0, qApp, SLOT(quit()));
		} else {
			QTimer::singleShot(0, m_manager, SLOT(initExtensions()));
		}
	} else if (m_singleProfile) {
		QTimer::singleShot(0, qApp, SLOT(quit()));
	}
	QWizard::done(result);
}
}
