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
#include "plugman.h"
#include "packagemodel.h"
#include "packagedownloaddialog.h"
#include <qutim/debug.h>
#include <QAction>
#include <QDir>
#include <QProgressBar>
#include <QProcess>
#include <QNetworkReply>
#include <qdeclarative.h>
#include <qutim/icon.h>
#include <qutim/menucontroller.h>
#include <qutim/actiongenerator.h>
#include <qutim/servicemanager.h>
#include <qutim/systeminfo.h>
#include <qutim/systemintegration.h>
#include <attica/content.h>
#include <attica/downloaditem.h>

#include <QListView>

using namespace qutim_sdk_0_3;
using namespace PackageManager;
using namespace Attica;

void PackageManagerPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Package manager"),
	        QT_TRANSLATE_NOOP("Plugin", "Provides possibility to Get Hot New Stuff from open community"),
	        PLUGIN_VERSION(0, 0, 0, 1),
	        ExtensionIcon());
	setCapabilities(Loadable);
	addAuthor(QLatin1String("euroelessar"));
//	addAuthor(QLatin1String("sauron"));
	qmlRegisterType<PackageModel>("org.qutim.plugman", 0, 3, "PackageModel");
}

bool PackageManagerPlugin::load()
{
	m_gen = new ActionGenerator(qutim_sdk_0_3::Icon("download"),
	                            QT_TRANSLATE_NOOP("PackageManager", "Manage packages"),
	                            this, SLOT(onManagerButtonClicked()));
	if (MenuController *menu = ServiceManager::getByName<MenuController*>("ContactList"))
		menu->addAction(m_gen);
//	m_emoticonsEngine = new PackageEngine(QStringList(QLatin1String("Emoticon Theme")),
//	                                      QLatin1String("emoticons"));
//	m_stylesEngine = new PackageEngine(QStringList(QLatin1String("Kopete Style 0.11"))
//	                                   << QLatin1String("Kopete Style 0.12+"),
//	                                   QLatin1String("emoticons"));
	return true;
}

bool PackageManagerPlugin::unload()
{
	if (MenuController *menu = ServiceManager::getByName<MenuController*>("ContactList"))
		return menu->removeAction(m_gen);
	return false;
}

void PackageManagerPlugin::onManagerButtonClicked()
{
	QDialog *dialog = new PackageDownloadDialog(QStringList(QLatin1String("Emoticon Theme")),
	                                            QLatin1String("emoticons"));
    SystemIntegration::show(dialog);
}

QUTIM_EXPORT_PLUGIN(PackageManager::PackageManagerPlugin)

