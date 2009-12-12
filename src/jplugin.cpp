#include "jplugin.h"
#include "protocol/jprotocol.h"
#include "protocol/jaccountwizard.h"

namespace Jabber
{
	JPlugin::JPlugin()
	{
		qDebug("%s", Q_FUNC_INFO);
	}

	void JPlugin::init()
	{
		qDebug("%s", Q_FUNC_INFO);
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Jabber"),
			QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"),
			PLUGIN_VERSION(0, 0, 1, 0),
			ExtensionIcon("im-jabber"));
		addAuthor(QT_TRANSLATE_NOOP("Author", "Denis Daschenko"),
			QT_TRANSLATE_NOOP("Task", "Author"),
			QLatin1String("daschenko@gmail.com"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber"),
			QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"),
			new GeneralGenerator<JProtocol>(),
			ExtensionIcon("im-jabber"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Account creator for Jabber protocol"),
			new GeneralGenerator<JAccountWizard>(),
			ExtensionIcon("im-jabber"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "LiveJournal account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Account creator for LiveJournal"),
			new GeneralGenerator<LJAccountWizard>(),
			ExtensionIcon("im-livejournal"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Google Talk account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Account creator for Google Talk"),
			new GeneralGenerator<GTAccountWizard>(),
			ExtensionIcon("im-gtalk"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Yandex.Online account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Account creator for Yandex.Online"),
			new GeneralGenerator<YAccountWizard>(),
			ExtensionIcon("im-yandex"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "QIP account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Account creator for QIP"),
			new GeneralGenerator<QIPAccountWizard>(),
			ExtensionIcon("im-qip"));
	}

	bool JPlugin::load()
	{
		return true;
	}

	bool JPlugin::unload()
	{
		return false;
	}

} // Jabber namespace

QUTIM_EXPORT_PLUGIN(Jabber::JPlugin)
