/****************************************************************************
 *  quetzalprotocol.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "quetzalprotocol.h"
#include "quetzalaccount.h"
#include "quetzalplugin.h"
#include "quetzalaccountwizard.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/statusactiongenerator.h>
#include <qutim/systeminfo.h>

QuetzalProtocol::QuetzalProtocol(const QuetzalMetaObject *meta, PurplePlugin *plugin)
{
	QObject::d_ptr->metaObject = const_cast<QuetzalMetaObject *>(meta);
	m_plugin = plugin;
	protocols().insert(m_plugin, this);
}

QuetzalProtocol::~QuetzalProtocol()
{
	delete static_cast<const QuetzalMetaObject *>(QObject::d_ptr->metaObject);
	QObject::d_ptr->metaObject = 0;
	protocols().remove(m_plugin);
}

QList<Account *> QuetzalProtocol::accounts() const
{
	QList<QuetzalAccount *> accounts = m_accounts.values();
	return *reinterpret_cast<QList<Account *> *>(&accounts);
}

Account *QuetzalProtocol::account(const QString &id) const
{
	return m_accounts.value(id);
}

void initActions()
{
	static bool inited = false;
	if (inited)
		return;
	QList<ActionGenerator *> actions;
	actions << new StatusActionGenerator(Status(Status::Online))
			<< new StatusActionGenerator(Status(Status::FreeChat))
			<< new StatusActionGenerator(Status(Status::Away))
			<< new StatusActionGenerator(Status(Status::NA))
			<< new StatusActionGenerator(Status(Status::DND))
			<< new StatusActionGenerator(Status(Status::Offline));
	foreach (ActionGenerator *action, actions)
		MenuController::addAction(action, &QuetzalAccount::staticMetaObject);
	inited = true;
	QString path = SystemInfo::getPath(SystemInfo::ConfigDir);
	path += "/purple";
	purple_util_set_user_dir(QDir::toNativeSeparators(path).toUtf8().constData());
}

void QuetzalProtocol::loadAccounts()
{
	initActions();
	QStringList accounts = config("general").value("accounts", QStringList());
	debug() << id() << accounts;
	foreach(const QString &id, accounts) {
		QuetzalAccount *account = new QuetzalAccount(id, this);
		m_accounts.insert(id, account);
		emit accountCreated(account);
	}
}

QByteArray quetzal_fix_protocol_name(const char *name)
{
	if (!qstrcmp(name, "XMPP"))
		return "jabber";
	return QByteArray(name).toLower();
}

QuetzalMetaObject::QuetzalMetaObject(PurplePlugin *protocol)
{
	QByteArray stringdata_b = "Quetzal::Protocol::";
	stringdata_b += protocol->info->id;
	stringdata_b += '\0';
	stringdata_b.replace('-', '_');
	int value = stringdata_b.size();
	stringdata_b += quetzal_fix_protocol_name(protocol->info->name);
	stringdata_b += '\0';
	int key = stringdata_b.size();
	stringdata_b.append("Protocol\0", 9);

	char *stringdata = (char*)qMalloc(stringdata_b.size() + 1);
	uint *data = (uint*) calloc(17, sizeof(uint));
	qMemCopy(stringdata, stringdata_b.constData(), stringdata_b.size() + 1);
	data[0] = 4;
	data[2] = 1;
	data[3] = 14;
	data[14] = key;
	data[15] = value;

	d.superdata = &QuetzalProtocol::staticMetaObject;
	d.stringdata = stringdata;
	d.data = data;
	d.extradata = 0;
}

QuetzalMetaObject::QuetzalMetaObject(QuetzalProtocolGenerator *protocol)
{
	const QMetaObject *meta = protocol->metaObject();
	QByteArray stringdata_b = "Quetzal::AccountWizard::";
	stringdata_b += protocol->plugin()->info->id;
	stringdata_b += '\0';
	stringdata_b.replace('-', '_');
	int value = stringdata_b.size();
	stringdata_b += meta->className();
	stringdata_b += '\0';
	int key = stringdata_b.size();
	stringdata_b += "DependsOn";
	stringdata_b += '\0';

	char *stringdata = (char*)qMalloc(stringdata_b.size() + 1);
	uint *data = (uint*) calloc(17, sizeof(uint));
	qMemCopy(stringdata, stringdata_b.constData(), stringdata_b.size() + 1);
	data[0] = 4;
	data[2] = 1;
	data[3] = 14;
	data[14] = key;
	data[15] = value;

	d.superdata = &QuetzalAccountWizard::staticMetaObject;
	d.stringdata = stringdata;
	d.data = data;
	d.extradata = 0;
}
