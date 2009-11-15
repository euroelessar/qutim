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
		KIcon kdeIcon("kde");
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
