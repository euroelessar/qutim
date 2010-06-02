#include "modulemanagerimpl.h"
#include "libqutim/jsonfile.h"
#include <QVariant>
#include <QFile>
#include <QDebug>
#include <QScriptValue>
#include <QScriptEngine>
#include <QTimer>
#include "libqutim/protocol.h"
#include "profiledialog.h"
#include "profilecreationwizard.h"
#include "libqutim/game/config.h"

namespace Core
{
	ModuleManagerImpl::ModuleManagerImpl()
	{
		loadPlugins();

		Game::Config config = ProfileDialog::profilesInfo();
#ifdef Q_OS_WIN
		if (config.value("singleProfile", false)) {
#else
		if (config.value("singleProfile", true)) {
#endif
			if (!config.hasChildGroup("profile")) {
				QWidget *wizard = new ProfileCreationWizard(this, QString(), QString(), true);
				wizard->setAttribute(Qt::WA_DeleteOnClose, true);
				wizard->setAttribute(Qt::WA_QuitOnClose, false);
#if	defined(Q_OS_SYMBIAN)
				wizard->showMaximized();
#else
				wizard->show();
#endif
			} else {
				config.beginGroup("profile");
				if(ProfileDialog::acceptProfileInfo(config, QString())) {
					QTimer::singleShot(0, this, SLOT(initExtensions()));
				} else {
					qWarning("Can't login");
				}
				config.endGroup();
			}
		} else {
			QDialog *dialog = new ProfileDialog(config, this);
#if	defined(Q_OS_SYMBIAN)
			dialog->showMaximized();
#else
			dialog->show();
#endif
		}
	}

	ExtensionInfoList ModuleManagerImpl::coreExtensions() const
	{
		return extensions();
	}

	void ModuleManagerImpl::initExtensions()
	{
		ModuleManager::initExtensions();
	}
}
