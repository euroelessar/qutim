#include "kdeintegration.h"
#include "emoticons/kdeemoticons.h"
#include "crash/crashplugin.h"
#include "speller/kdespellerlayer.h"
#include "speller/kdespellersettings.h"
#include "quetzalgui.h"
#include <kdeversion.h>
#include <KIcon>
#include <qutim/event.h>
#include <QCoreApplication>

namespace KdeIntegration
{
	KdePlugin::KdePlugin()
	{
		m_quetzal_id = Event::registerType("quetzal-ui-ops-inited");
		qApp->installEventFilter(this);
	}

	void KdePlugin::init()
	{
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
