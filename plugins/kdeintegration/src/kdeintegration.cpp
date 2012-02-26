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
#include "kdeintegration.h"
#include "emoticons/kdeemoticons.h"
#include "crash/crashplugin.h"
#include "speller/kdespellerlayer.h"
#include "speller/kdespellersettings.h"
#include "iconloader/kdeiconloader.h"
#include "tray/kdetrayicon.h"
#include "aboutkde/aboutkdedialog.h"
#include "aboutapp/kdeaboutappdialog.h"
#include "sysintegration/kdesystemintegration.h"
#include "quetzalgui.h"
#include <kdeversion.h>
#include <KIcon>
#include <KMenu>
#include <qutim/event.h>
#include <QCoreApplication>
#include <KAboutData>
#include <kcmdlineargs.h>
#include <QDebug>

namespace qutim_sdk_0_3
{
typedef QMenu *(*_menu_creator_hook)(const QString &title, QWidget *parent);
extern LIBQUTIM_EXPORT _menu_creator_hook menu_creator_hook;
}

namespace KdeIntegration
{
QMenu *kmenu_creator(const QString &title, QWidget *parent)
{
	KMenu *menu = new KMenu(title, parent);
	if (!title.isEmpty())
		menu->addTitle(title);
	return menu;
}

KdePlugin::KdePlugin()
{
	m_quetzal_id = Event::registerType("quetzal-ui-ops-inited");
	Event::eventManager()->installEventFilter(this);
}

void KdePlugin::init()
{
	//		QApplication::setPalette(KGlobalSettings::createApplicationPalette());
	menu_creator_hook = kmenu_creator;
	KAboutData aboutData(QCoreApplication::applicationName().toUtf8(),
						 QByteArray(),
						 ki18n("qutIM Instant Messenger"),
						 QCoreApplication::applicationVersion().toUtf8());
	aboutData.setProgramIconName(QLatin1String("qutim"));
	aboutData.setHomepage("http://qutim.org");

//	aboutData.addAuthor(ki18n("Ruslan Nigmatullin"), ki18n("Developer"), "euroelessar@yandex.ru");
//	aboutData.addAuthor(ki18n("Aleksey Sidorov"), ki18n("Developer"), "gorthauer87@yandex.ru");
	aboutData.setShortDescription(ki18n("Communicate over IM"));
	aboutData.addLicense(KAboutData::License_GPL_V2);
	aboutData.setBugAddress("https://bugs.launchpad.net/qutim/+filebug");
	aboutData.setOrganizationDomain(QCoreApplication::organizationDomain().toUtf8());
	KGlobal::setActiveComponent(KComponentData(aboutData));

	// Don't know better way to know application arguments from here
	QStringList args = QCoreApplication::arguments();
	int argc = args.count();
	char **argv = (char **) malloc(sizeof(char *) * argc);
	for (int i = 0; i < argc; i++)
		argv[i] = qstrdup(args.at(i).toLocal8Bit().constData());
	// Whe shouldn't free memory because KCmdLineArgs stores pointers internally to it
	KCmdLineArgs::init(argc, argv, KGlobal::activeComponent().aboutData());

	ExtensionIcon kdeIcon("kde");
	addAuthor(QLatin1String("euroelessar"));
	addAuthor(QLatin1String("sauron"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "KDE integration"),
			QT_TRANSLATE_NOOP("Plugin", "Integration with K Desktop Environment"),
			KDE_VERSION << 8, kdeIcon);
	addExtension<KdeSystemIntegration>(QT_TRANSLATE_NOOP("Plugin", "KDE Integration"),
	                                   QT_TRANSLATE_NOOP("Plugin", "Full-featured integration with KDE"),
	                                   kdeIcon);
	addExtension<KdeEmoticons>(QT_TRANSLATE_NOOP("Plugin", "KDE Emoticons"),
							   QT_TRANSLATE_NOOP("Plugin", "Using KDE emoticons packs"),
							   kdeIcon);
	addExtension<KdeCrashHandler>(QT_TRANSLATE_NOOP("Plugin", "KDE Crash handler"),
								  QT_TRANSLATE_NOOP("Plugin", "Handles qutIM's craches by KCrash"),
								  kdeIcon);
	addExtension<KdeSpellerLayer>(QT_TRANSLATE_NOOP("Plugin", "KDE Spell checker"),
								  QT_TRANSLATE_NOOP("Plugin", ""),
								  kdeIcon);
	addExtension<KdeIconLoader>(QT_TRANSLATE_NOOP("Plugin", "KDE Icon engine"),
								QT_TRANSLATE_NOOP("Plugin", "Uses KDE Icon engine for providing icons"),
								kdeIcon);
	//deprecated
	//addExtension<KDENotificationLayer>(QT_TRANSLATE_NOOP("Plugin", "KDE Notifications"),
	//								   QT_TRANSLATE_NOOP("Plugin", "Uses KDE notifications"),
	//								   kdeIcon);
	addExtension<KdeTrayIcon>(QT_TRANSLATE_NOOP("Plugin", "KDE Status Notifier"),
							  QT_TRANSLATE_NOOP("Plugin", "Using new KDE DBus tray specification"),
							  kdeIcon);
	addExtension<AboutKdeDialog, StartupModule>(QT_TRANSLATE_NOOP("Plugin", "About KDE dialog"),
	                                            QT_TRANSLATE_NOOP("Plugin", "Show \"About KDE\" menu option"),
	                                            kdeIcon);
	addExtension<KdeAboutAppDialog>(QT_TRANSLATE_NOOP("Plugin", "KDE About application dialog"),
	                                QT_TRANSLATE_NOOP("Plugin", "Use kde-specific application about dialog"),
	                                kdeIcon);
}

bool KdePlugin::load()
{
	return true;
}

bool KdePlugin::unload()
{
	return false;
}

bool KdePlugin::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == Event::eventType()) {
		Event *event = static_cast<Event *>(ev);
		if (event->id == m_quetzal_id) {
			Event::eventManager()->removeEventFilter(this);
			Quetzal::initGui();
		}
	}
	return QObject::eventFilter(obj, ev);
}

Q_GLOBAL_STATIC(KHelpMenu, kdeHelpMenu)

KHelpMenu *KdePlugin::helpMenu()
{
	return kdeHelpMenu();
}
}

QUTIM_EXPORT_PLUGIN(KdeIntegration::KdePlugin)

