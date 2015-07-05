/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "clconfplugin.h"
#include <qutim/servicemanager.h>

ClConfPlugin *ClConfPlugin::self;

ClConfPlugin::ClConfPlugin()
{
	Q_ASSERT(!self);
	self = this;
}

void ClConfPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Conferences in contact list"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin adds your conferences to the contact list"),
			PLUGIN_VERSION(0, 0, 1, 0),
			ExtensionIcon());
	setCapabilities(Loadable);
}

bool ClConfPlugin::load()
{
	foreach (Protocol *protocol, Protocol::all()) {
		foreach (Account *account, protocol->accounts())
			onAccountCreated(account);
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}
	return true;
}

bool ClConfPlugin::unload()
{
	return false;
}

void ClConfPlugin::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	m_accounts.insert(account, new ProxyAccount(account));
	connect(account, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			SLOT(onConferenceCreated(qutim_sdk_0_3::Conference*)));
	connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDestroyed(QObject*)));
	foreach (Conference *conference, account->findChildren<Conference*>())
		onConferenceCreated(conference);
}

void ClConfPlugin::onAccountDestroyed(QObject *account)
{
	m_accounts.take(account)->deleteLater();
}

void ClConfPlugin::onConferenceCreated(qutim_sdk_0_3::Conference *conference)
{
	QObject *cl = ServiceManager::getByName("ContactList");
	if (cl) {
		ProxyContact *contact = new ProxyContact(conference);
		cl->metaObject()->invokeMethod(cl, "addContact",
									   Q_ARG(qutim_sdk_0_3::Contact*, contact));
	}
}

QUTIM_EXPORT_PLUGIN(ClConfPlugin)

