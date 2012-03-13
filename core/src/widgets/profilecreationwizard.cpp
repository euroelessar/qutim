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

#include "profilecreationwizard.h"
#include "submitpage.h"
#include <qutim/profilecreatorpage.h>
#include <qutim/objectgenerator.h>
#include <qutim/systeminfo.h>
#include <qutim/profile.h>
#include "profilecreationpage.h"
#include <qutim/jsonfile.h>
#include <qutim/config.h>
#include <qutim/debug.h>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include <QLibrary>
#ifdef Q_OS_UNIX
# include <pwd.h>
# include <unistd.h>
#elif defined( Q_OS_WIN )
# define SECURITY_WIN32
# include <windows.h>
# include <lmcons.h>
# include <security.h>
#endif

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

QString randomString(int len)
{
	char symbols[26 * 2 + 10];
	int size = sizeof(symbols) / sizeof(char);
	for (char c = 'a'; c <= 'z'; ++c) {
		symbols[c - 'a'] = c;
		symbols[c - 'a' + 26] = c -'a' + 'A';
	}
	for (int i = 0; i < 10; i++)
		symbols[26 * 2 + i] = '0' + i;
	QString str(len, Qt::Uninitialized);
	for (int i = 0; i < len; ++i)
		str[i] = symbols[qrand() % size];
	return str;
}

ProfileCreationWizard::ProfileCreationWizard(ModuleManager *parent,
											 const QString &id, const QString &password,
											 bool singleProfile)
{
	m_manager = parent;
	m_singleProfile = singleProfile;
	
	QDir tmpDir = QDir::temp();
	QString path;
	do {
		path = "qutim-" + randomString(6) + "-profile";
	} while (tmpDir.exists(path));
	if (!tmpDir.mkpath(path) || !tmpDir.cd(path)) {
		qFatal("Can't access or create directory '%s'",
			   qPrintable(tmpDir.absoluteFilePath(path)));
	}
	QVector<QDir> &systemDirs = *system_info_dirs();
	systemDirs[SystemInfo::ConfigDir] = tmpDir.absoluteFilePath("config");
	systemDirs[SystemInfo::HistoryDir] = tmpDir.absoluteFilePath("history");
	systemDirs[SystemInfo::ShareDir] = tmpDir.absoluteFilePath("share");
	for (int i = SystemInfo::ConfigDir; i <= SystemInfo::ShareDir; i++)
		tmpDir.mkdir(systemDirs[i].dirName());
	debug() << Q_FUNC_INFO << SystemInfo::getPath(SystemInfo::ConfigDir);
	
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

	SubmitPage *p = new SubmitPage(new StatisticsHelper(), this);
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
	if (result == Accepted) {
		ProfileCreationPage *page = findChild<ProfileCreationPage*>();
		QList<ConfigBackend*> &configBackends = get_config_backends();
		{
			Config config = Profile::instance()->config();
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
		QTimer::singleShot(0, m_manager, SLOT(initExtensions()));
	} else if (m_singleProfile) {
		QTimer::singleShot(0, qApp, SLOT(quit()));
	}
	QWizard::done(result);
}
}

