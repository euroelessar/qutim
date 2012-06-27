/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "applicationwindowplugin.h"
#include "applicationwindow.h"
#include "chat.h"
#include "contactlist.h"

#include "quickpassworddialog.h"
#include "quickauthdialog.h"
#include "quickaddcontactdialog.h"
#include "quickaboutdialog.h"
#include "maemo6idle.h"
#include "quickjoingroupchat.h"
#include "accountsmodel.h"
#include "quicksettingslayer.h"
#include "quicknotificationmanager.h"
#include "quickaddaccountdialog.h"
#include "addaccountdialogwrapper.h"
#include "meegointegration.h"
#include "quickiconloader.h"

namespace MeegoIntegration
{
	void Plugin::init()
	{
		qutim_sdk_0_3::LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Meego application window");
		qutim_sdk_0_3::LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Main window of application");
		setInfo(name, description, PLUGIN_VERSION(0, 1, 0, 0));
		addExtension<ApplicationWindow>(name, description);
		addExtension<Chat>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego chat page"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple chat based on QML elements"));
		addExtension<ContactList>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego contact list page"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple contact list on QML elements"));
		addExtension<QuickPasswordDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego password dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple password dialog on QML elements"));
		addExtension<QuickAuthDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego authorization dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple authorization dialog on QML elements"));
		addExtension<Maemo6Idle>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo Harmattan Idle detector"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple harmattan idle detector"));
		addExtension<QuickAddContactDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo Add contact dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple add contact dialog"));
		addExtension<QuickAboutDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo about dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple about dialog"));
		addExtension<QuickJoinGroupChat>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo join group chat dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple join group chat dialog"));
		addExtension<MeeGoIntegration>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo integration"),
			    QT_TRANSLATE_NOOP("Plugin", "Provider for better integration with the system"));
		addExtension<AccountsModel>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGoAccounts Model"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple accounts model"));
		addExtension<QuickNoficationManager>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo Notification Manager"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple notification manager"));
		addExtension<QuickSettingsLayer>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo settings"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple MeeGo settings dialog"));
		addExtension<QuickAddAccountDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo account creator"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple MeeGo account creator"));
		addExtension<QuickIconLoader>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo icon loader noop"),
			    QT_TRANSLATE_NOOP("Plugin", "Noop for icon loader service"));
	}
}

QUTIM_EXPORT_PLUGIN(MeegoIntegration::Plugin)

