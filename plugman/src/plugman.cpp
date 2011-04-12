#include "plugman.h"
#include "packagemodel.h"
#include "packagedownloaddialog.h"
#include <qutim/debug.h>
#include <QAction>
#include <QDir>
#include <QProgressBar>
#include <QProcess>
#include <QNetworkReply>
#include <qutim/icon.h>
#include <qutim/menucontroller.h>
#include <qutim/actiongenerator.h>
#include <qutim/servicemanager.h>
#include <qutim/systeminfo.h>
#include <attica/content.h>
#include <attica/downloaditem.h>
#include <quasar/tar.h>
#include <quasar/zip.h>

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
	addAuthor(QLatin1String("euroelessar"));
//	addAuthor(QLatin1String("sauron"));
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
	PackageEngine *engine = new PackageEngine(QStringList(QLatin1String("Emoticon Theme")),
	                                          QLatin1String("emoticons"));
	QDialog *dialog = new PackageDownloadDialog(engine);
	engine->setParent(dialog);
	dialog->show();
}

QUTIM_EXPORT_PLUGIN(PackageManager::PackageManagerPlugin)
