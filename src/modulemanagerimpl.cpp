#include "modulemanagerimpl.h"
#include <qutim/jsonfile.h>
#include <QVariant>
#include <QFile>
#include <QDebug>
#include <QScriptValue>
#include <QScriptEngine>
#include <QTimer>
#include <qutim/protocol.h>
#include "profiledialog.h"
#include "profilecreationwizard.h"
#include <qutim/config.h>
#include <qutim/notificationslayer.h>
#include <qutim/systemintegration.h>

#include <qutim/debug.h>
#include <qutim/systeminfo.h>

namespace Core
{
ModuleManagerImpl::ModuleManagerImpl()
{
	ModuleManager::loadPlugins();
	Config config = ProfileDialog::profilesInfo();
#ifdef QUTIM_SINGLE_PROFILE
	bool singleProfile = true;
#else
	bool singleProfile = false;
#endif
	singleProfile = config.value("singleProfile", singleProfile);
	if (singleProfile) {
		if (!config.hasChildGroup("profile")) {
			QWidget *wizard = new ProfileCreationWizard(this, QString(), QString(), true);
			wizard->setAttribute(Qt::WA_DeleteOnClose, true);
			wizard->setAttribute(Qt::WA_QuitOnClose, false);
			SystemIntegration::show(wizard);
		} else {
			config.beginGroup("profile");
			if(ProfileDialog::acceptProfileInfo(config, QString())) {
				QTimer::singleShot(0, this, SLOT(initExtensions()));
			} else {
				qWarning("Can't login");
				QDialog *dialog = new ProfileDialog(config, this);
				SystemIntegration::show(dialog);
			}
			config.endGroup();
		}
	} else {
		QDialog *dialog = new ProfileDialog(config, this);
		SystemIntegration::show(dialog);
	}
}

ExtensionInfoList ModuleManagerImpl::coreExtensions() const
{
	return extensions();
}

void ModuleManagerImpl::initExtensions()
{
	debug() << SystemInfo::getPath(SystemInfo::SystemConfigDir);
	ModuleManager::initExtensions();

	NotificationRequest request(Notification::AppStartup);
	request.send();
}
}
