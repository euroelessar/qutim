#include "profilecreationwizard.h"
#include <qutim/profilecreatorpage.h>
#include <qutim/objectgenerator.h>
#include <qutim/systeminfo.h>
#include "profilecreationpage.h"
#include <qutim/jsonfile.h>
#include <qutim/config.h>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include <QLibrary>
#ifdef Q_OS_UNIX
# include <pwd.h>
#elif defined( Q_OS_WIN )
# define SECURITY_WIN32
# include <windows.h>
# include <lmcons.h>
# include <security.h>
#endif

#include "submitpage.h"

namespace qutim_sdk_0_3
{
	LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs();
	LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();
}

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
	
	setProperty("singleProfile",singleProfile);
	setProperty("password",password);

	addPage(new ProfileCreationPage(this));
	QString realId;
	QString realName;
	if (id.isEmpty()) {
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
		QT_TRY {
			struct passwd *userInfo = getpwuid(getuid());
			QTextCodec *codec = QTextCodec::codecForLocale();
			realId = codec->toUnicode(userInfo->pw_name);
			realName = codec->toUnicode(userInfo->pw_gecos).section(',', 0, 0);
		} QT_CATCH(...) {
		}
#elif defined(Q_OS_WIN32)
//		TCHAR wTId[UNLEN + 1];
//		DWORD wSId = sizeof(wTId);
//		if (GetUserName(wTId, &wSId))
//			#if defined(UNICODE)
//			realid= QString::fromUtf16((ushort*)wTId);
//			#else
//			realId = QString::fromLocal8Bit(wTId);
//			#endif

//		TCHAR wTName[1024];
//		DWORD wSName = 1024;
//		if (GetUserNameEx(NameDisplay, wTName, &wSName))
//			#if defined( UNICODE )
//			realName = QString::fromUtf16((ushort*)wTName);
//			#else
//			realName = QString::fromLocal8Bit(wTName);
//			#endif
#endif
		if (realName.isEmpty())
			realName = realId;
	} else {
		realId = id;
		realName = id;
	}
	setField("id", realId);
	setField("name", realName);
	QList<ProfileCreatorPage *> creators;
	foreach (const ObjectGenerator *gen, ObjectGenerator::module<ProfileCreatorPage>()) {
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

	SubmitPage *p = new SubmitPage(this);
	addPage(p);

	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);

	//back button is unstable
	QList<QWizard::WizardButton> layout;
	layout << QWizard::Stretch << QWizard::NextButton << QWizard::FinishButton;
	setButtonLayout(layout);
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
		QList<ConfigBackend*> &configBackends = get_config_backends();
		QVariant var;
		if (file.load(var))
			map = var.toMap();
		{
			Config config(&map);
			if (m_singleProfile) {
				config.beginGroup("profile");
			} else {
				int size = config.beginArray("list");
				config.setArrayIndex(size);
			}
			config.setValue("name", field("name"));
			config.setValue("id", field("id"));
			config.setValue("crypto", QLatin1String(page->cryptoName()));
			config.setValue("config", QLatin1String(configBackends.first()->metaObject()->className()));
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
