#include "kdeintegration.h"
#include "emoticons/kdeemoticons.h"
#include <kdeversion.h>
#include <KIcon>

namespace KdeIntegration
{
	KdePlugin::KdePlugin()
	{
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
	}

	bool KdePlugin::load()
	{
		return true;
	}

	bool KdePlugin::unload()
	{
		return false;
	}
}

QUTIM_EXPORT_PLUGIN(KdeIntegration::KdePlugin)
