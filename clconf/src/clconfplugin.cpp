/****************************************************************************
 *  clconfplugin.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/

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
