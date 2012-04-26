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
#include "jplugin.h"
#include "protocol/jprotocol.h"
#include "protocol/jaccountwizard.h"
#include "protocol/modules/vcard/jvcardmanager.h"
#include "protocol/modules/servicediscovery/jservicebrowser.h"
#include "protocol/modules/xmlconsole/xmlconsole.h"
#include "protocol/modules/adhoc/jadhocmodule.h"
#include "protocol/modules/filetransfer/jfiletransfer.h"
#include "protocol/modules/remotecontrol/remotecontrol.h"
#include "protocol/modules/pep/jpersoneventsupport.h"
#include "protocol/modules/moodpep/jpersonmoodconverter.h"
#include "protocol/modules/moodpep/jmoodchooser.h"
#include "protocol/modules/tunepep/jpersontuneconverter.h"
#include "protocol/modules/activitypep/jpersonactivityconverter.h"
#include "protocol/modules/activitypep/jactivitychooser.h"
#include "protocol/modules/proxy/jproxymanager.h"
#include "protocol/modules/jinglesupport/jinglesupport.h"

namespace Jabber
{
JPlugin::JPlugin()
{
}

void JPlugin::init()
{
	qRegisterMetaType<QSet<QString> >("QSet<QString>");
	ExtensionIcon jabberIcon("im-jabber");
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Jabber"),
			QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"),
			PLUGIN_VERSION(0, 0, 1, 0),
			jabberIcon);
	addAuthor(QLatin1String("reindeer"));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Nikita Belov"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("zodiac.nv@gmail.com"));
	addAuthor(QLatin1String("euroelessar"));
	addAuthor(QLatin1String("sauron"));
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
#ifndef Q_WS_S60
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber XML console"),
				 QT_TRANSLATE_NOOP("Plugin", "XML console for low-level protocol debugging"),
				 new GeneralGenerator<XmlConsole, JabberExtension>(),
				 ExtensionIcon("utilities-terminal"));
#endif
	//		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber AdHoc"),
	//			QT_TRANSLATE_NOOP("Plugin", "Implementation of Ad-hoc Commands"),
	//			new GeneralGenerator<JAdHocModule, JabberExtension>(),
	//			ExtensionIcon("utilities-terminal"));
	//		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber Remote Control"),
	//			QT_TRANSLATE_NOOP("Plugin", "Implementation of client remote controlling"),
	//			new GeneralGenerator<RemoteControl, JabberExtension>(),
	//			ExtensionIcon("utilities-terminal"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber Service Browser"),
				 QT_TRANSLATE_NOOP("Plugin", "Implementation of service browser"),
				 new GeneralGenerator<JServiceBrowserModule, JabberExtension>(),
				 ExtensionIcon("services"));
	//		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber File Transfer"),
	//			QT_TRANSLATE_NOOP("Plugin", "Implementation of XEP-0096"),
	//			new GeneralGenerator<JFileTransferFactory>(),
	//			ExtensionIcon("document-save"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber Personal Events support"),
				 QT_TRANSLATE_NOOP("Plugin", "Implementation of personal events protocol"),
				 new GeneralGenerator<JPersonEventSupport, JabberExtension>(),
				 ExtensionIcon(""));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber VCard support"),
				 QT_TRANSLATE_NOOP("Plugin", "Implementation of VCard info requests"),
				 new GeneralGenerator<JVCardManager, JabberExtension>(),
				 ExtensionIcon(""));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber User Mood support"),
				 QT_TRANSLATE_NOOP("Plugin", "Convertes internal XMPP data of mood to Qt data types"),
				 new GeneralGenerator<JPersonMoodConverter, PersonEventConverter>(),
				 ExtensionIcon(""));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber Mood Chooser"),
				 QT_TRANSLATE_NOOP("Plugin", "Provides the dialog to set your mood"),
				 new GeneralGenerator<JMoodChooser, JabberExtension>(),
				 ExtensionIcon(""));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber User Tune support"),
				 QT_TRANSLATE_NOOP("Plugin", "Convertes internal XMPP data of tune to Qt data types"),
				 new GeneralGenerator<JPersonTuneConverter, PersonEventConverter>(),
				 ExtensionIcon(""));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber User Activity support"),
				QT_TRANSLATE_NOOP("Plugin", "Convertes internal XMPP data of activity to Qt data types"),
				new GeneralGenerator<JPersonActivityConverter, PersonEventConverter>(),
				ExtensionIcon(""));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber Activity Chooser"),
				QT_TRANSLATE_NOOP("Plugin", "Provides the dialog to set your activity"),
				new GeneralGenerator<JActivityChooser, JabberExtension>(),
				ExtensionIcon(""));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber proxy support"),
				 QT_TRANSLATE_NOOP("Plugin", "Jabber proxy support"),
				 new SingletonGenerator<JProxyManager, NetworkProxyManager>(),
				 ExtensionIcon("im-jabber"));
#if 0
#ifdef JABBER_HAVE_MULTIMEDIA
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber VoIP support"),
				 QT_TRANSLATE_NOOP("Plugin", "Allows to talk by voice and video"),
				 new GeneralGenerator<JingleSupport, JabberExtension>(),
				 ExtensionIcon(""));
#endif
#endif
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

