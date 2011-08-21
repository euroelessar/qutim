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
** the Free Software Foundation, either version 2 of the License, or
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
#include <QDebug>
#include <QScriptValue>
#include <QScriptEngine>
#include <QTimer>
#include <QSslSocket>
#include <qutim/protocol.h>
#include "profiledialog.h"
#include "profilecreationwizard.h"
#include <qutim/config.h>
#include <qutim/notification.h>
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
	return ExtensionInfoList();
}

void ModuleManagerImpl::initExtensions()
{
	QString path = SystemInfo::getPath(SystemInfo::SystemShareDir);
	path += QLatin1String("/ca-certs/*.pem");
	qDebug() << QSslSocket::defaultCaCertificates().size();
	qDebug() << QSslSocket::addDefaultCaCertificates(path, QSsl::Pem, QRegExp::Wildcard);
	qDebug() << QSslSocket::defaultCaCertificates().size();
	
	qDebug() << SystemInfo::getPath(SystemInfo::SystemConfigDir);
	ModuleManager::initExtensions();

	NotificationRequest request(Notification::AppStartup);
	request.send();
}
}
