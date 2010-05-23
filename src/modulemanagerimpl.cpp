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

namespace Core
{
	ModuleManagerImpl::ModuleManagerImpl()
	{
		loadPlugins();
		
		QVariantMap value = ProfileDialog::profilesInfo();
		bool singleProfile = value.value("singleProfile",
#ifdef Q_OS_WIN
										 false
#else
										 true
#endif
										 ).toBool();
		if (singleProfile) {
			QVariantMap map = value.value("profile").toMap();
			if (map.isEmpty()) {
				QWidget *wizard = new ProfileCreationWizard(this, QString(), QString(), true);
#if	defined(Q_OS_SYMBIAN)
				wizard->showMaximized();
#else
				wizard->show();
#endif
			} else if(ProfileDialog::acceptProfileInfo(map, QString())) {
				QTimer::singleShot(0, this, SLOT(initExtensions()));
			} else {
				qWarning("Can't login");
			}
		} else {
			QDialog *dialog = new ProfileDialog(value, this);
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
