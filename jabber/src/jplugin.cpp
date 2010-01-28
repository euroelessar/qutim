#include "jplugin.h"
#include "protocol/jprotocol.h"
#include "protocol/jaccountwizard.h"
#include "protocol/account/servicediscovery/jservicebrowser.h"
#include "protocol/modules/xmlconsole/xmlconsole.h"
#include "protocol/modules/adhoc/jadhocmodule.h"
#include "protocol/modules/remotecontrol/remotecontrol.h"

namespace Jabber
{
	JPlugin::JPlugin()
	{
		qDebug("%s", Q_FUNC_INFO);
	}

	void JPlugin::init()
	{
		qDebug("%s", Q_FUNC_INFO);
		ExtensionIcon jabberIcon("im-jabber");
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Jabber"),
			QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"),
			PLUGIN_VERSION(0, 0, 1, 0),
			jabberIcon);
		addAuthor(QT_TRANSLATE_NOOP("Author", "Denis Daschenko"),
			QT_TRANSLATE_NOOP("Task", "Author"),
			QLatin1String("daschenko@gmail.com"));
		addAuthor(QT_TRANSLATE_NOOP("Author", "Nikita Belov"),
			QT_TRANSLATE_NOOP("Task", "Author"),
			QLatin1String("zodiac.nv@gmail.com"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber"),
			QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"),
			new GeneralGenerator<JProtocol>(),
			jabberIcon);
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Account creator for Jabber protocol"),
			new GeneralGenerator<JAccountWizard>(),
			jabberIcon);
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
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber XML console"),
			QT_TRANSLATE_NOOP("Plugin", "XML console for low-level protocol debugging"),
			new GeneralGenerator<XmlConsole, JabberExtension>(),
			ExtensionIcon("utilities-terminal"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber AdHoc"),
			QT_TRANSLATE_NOOP("Plugin", "Implementation of Ad-hoc Commands"),
			new GeneralGenerator<JAdHocModule, JabberExtension>(),
			ExtensionIcon("utilities-terminal"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber Remote Control"),
			QT_TRANSLATE_NOOP("Plugin", "Implementation of client remote controlling"),
			new GeneralGenerator<RemoteControl, JabberExtension>(),
			ExtensionIcon("utilities-terminal"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber Service Browser"),
			QT_TRANSLATE_NOOP("Plugin", "Implementation of service browser"),
			new GeneralGenerator<JServiceBrowserModule, JabberExtension>(),
			ExtensionIcon("services"));
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
