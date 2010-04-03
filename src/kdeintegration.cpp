#include "kdeintegration.h"
#include "emoticons/kdeemoticons.h"
#include "crash/crashplugin.h"
#include "speller/kdespellerlayer.h"
#include "speller/kdespellersettings.h"
#include "iconloader/kdeiconloader.h"
#include "notification/kdenotificationlayer.h"
#include "quetzalgui.h"
#include <kdeversion.h>
#include <KIcon>
#include <qutim/event.h>
#include <QCoreApplication>
#include <KAboutData>

namespace KdeIntegration
{
	KdePlugin::KdePlugin()
	{
		m_quetzal_id = Event::registerType("quetzal-ui-ops-inited");
		qApp->installEventFilter(this);
	}

	void KdePlugin::init()
	{
//		QApplication::setPalette(KGlobalSettings::createApplicationPalette());
		KAboutData aboutData(QCoreApplication::applicationName().toUtf8(),
							  QByteArray(),
							  ki18n("qutIM Instant Messenger"),
							  QCoreApplication::applicationVersion().toUtf8());
		aboutData.setHomepage("http://qutim.org");

		aboutData.addAuthor(ki18n("Ruslan Nigmatullin"), ki18n("Developer"), "euroelessar@gmail.com");
		aboutData.addAuthor(ki18n("Aleksey Sidorov"), ki18n("Developer"), "sauron@citadelspb.com");
		aboutData.setShortDescription(ki18n("Communicate over IM"));
		aboutData.addLicense(KAboutData::License_GPL_V2);
		aboutData.addLicense(KAboutData::License_GPL_V3);
		aboutData.setBugAddress("euroelessar@ya.ru");
		aboutData.setOrganizationDomain(QCoreApplication::organizationDomain().toUtf8());
		KGlobal::setActiveComponent(KComponentData(aboutData));

		ExtensionIcon kdeIcon(KIcon("kde"));
		addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
				  QT_TRANSLATE_NOOP("Task", "Developer"),
				  QLatin1String("euroelessar@gmail.com"));
		addAuthor(QT_TRANSLATE_NOOP("Author", "Sidorov Aleksey"),
				  QT_TRANSLATE_NOOP("Task", "Developer"),
				  QLatin1String("sauron@citadelspb.com"));
		setInfo(QT_TRANSLATE_NOOP("Plugin", "KDE integration"),
				QT_TRANSLATE_NOOP("Plugin", "Integration with K Desktop Environment"),
				KDE_VERSION << 8, kdeIcon);
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
		addExtension<KDENotificationLayer>(QT_TRANSLATE_NOOP("Plugin", "KDE Notifications"),
										   QT_TRANSLATE_NOOP("Plugin", "Uses KDE notifications"),
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
				qApp->removeEventFilter(this);
				Quetzal::initGui();
			}
		}
		return QObject::eventFilter(obj, ev);
	}
}

QUTIM_EXPORT_PLUGIN(KdeIntegration::KdePlugin)
