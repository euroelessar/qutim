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

#include "quetzalprotocol.h"
#include "quetzalaccount.h"
#include "quetzalplugin.h"
#include "quetzalaccountwizard.h"
#include "quetzalaccountsettings.h"
#include <qutim/settingslayer.h>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/statusactiongenerator.h>
#include <qutim/systeminfo.h>

QuetzalProtocol::QuetzalProtocol(PurplePlugin *plugin)
{
	plugin->info->ui_info = this;
	m_plugin = plugin;
	protocols().insert(m_plugin, this);
}

QuetzalProtocol::~QuetzalProtocol()
{
	protocols().remove(m_plugin);
}

QList<Account *> QuetzalProtocol::accounts() const
{
	QList<Account*> accounts;
	QHash<QString, QuetzalAccount *>::const_iterator it = m_accounts.begin();
	for (; it != m_accounts.constEnd(); it++)
		accounts << it.value();
	return accounts;
}

Account *QuetzalProtocol::account(const QString &id) const
{
	return m_accounts.value(id);
}

QVariant QuetzalProtocol::data(DataType type)
{
	switch (type) {
	case ProtocolIdName:
		return QLatin1String("ID");
	case ProtocolContainsContacts: {
		PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_plugin);
		return info->add_buddy != NULL;
	}
	default:
		return QVariant();
	}
}

void QuetzalProtocol::removeAccount(Account *generalAccount, RemoveFlag flags)
{
	QuetzalAccount *account = qobject_cast<QuetzalAccount*>(generalAccount);
	if (!account)
		return;
	emit accountRemoved(account);
	purple_accounts_remove(account->purple());
	if (flags & DeleteAccount)
		delete account; // ->deleteLater();
}

void initActions()
{
	static bool inited = false;
	if (inited)
		return;
	Settings::registerItem<QuetzalAccount>(
			new GeneralSettingsItem<QuetzalAccountSettings>(
					Settings::Protocol,
					QIcon(),
					QT_TRANSLATE_NOOP_UTF8("Settings", "General"))
			);
	QList<ActionGenerator *> actions;
	actions << new StatusActionGenerator(Status(Status::Online))
			<< new StatusActionGenerator(Status(Status::FreeChat))
			<< new StatusActionGenerator(Status(Status::Away))
			<< new StatusActionGenerator(Status(Status::NA))
			<< new StatusActionGenerator(Status(Status::DND))
			<< new StatusActionGenerator(Status(Status::Offline));
	foreach (ActionGenerator *action, actions)
		MenuController::addAction(action, &QuetzalAccount::staticMetaObject);
//	QList<QLatin1String> ids;
//	foreach (Protocol *genericProto, Protocol::all()) {
//		if (QuetzalProtocol *proto = qobject_cast<QuetzalProtocol*>(genericProto))
//			ids << QLatin1String(proto->plugin()->info->id);
//	}
	GList *accounts = purple_accounts_get_all();
	for (; accounts; accounts = g_list_next(accounts)) {
		PurpleAccount *purpleAccount = reinterpret_cast<PurpleAccount*>(accounts->data);
		// Disable auto-connect for every account
		PurplePresence *presence = purpleAccount->presence;
		if (purple_presence_is_online(presence)) {
			purple_account_disconnect(purpleAccount);
			for (GList *it = purple_presence_get_statuses(presence); it; it = it->next) {
				PurpleStatus *status = reinterpret_cast<PurpleStatus *>(it->data);
				if (!purple_status_is_online(status)) {
					purple_presence_set_status_active(presence, purple_status_get_id(status), TRUE);
					break;
				}
			}
		}
//		const char *protocolId = purple_account_get_protocol_id(purpleAccount);
//		if (ids.contains(QLatin1String(protocolId)))
//			continue;
	}

	inited = true;
//	QString path = SystemInfo::getPath(SystemInfo::ConfigDir);
//	path += "/purple";
//	QByteArray nativePath = QDir::toNativeSeparators(path).toUtf8();
//	purple_util_set_user_dir(nativePath.constData());
//	path += "/icons";
//	nativePath = QDir::toNativeSeparators(path).toUtf8();
//	purple_buddy_icons_set_cache_dir(nativePath.constData());
}

void QuetzalProtocol::addAccount(PurpleAccount *purpleAccount)
{
	QuetzalAccount *account = new QuetzalAccount(purpleAccount, this);
	registerAccount(account);
	purple_accounts_add(purpleAccount);

//	Config cfg = config("general");
//	QStringList accounts = cfg.value("accounts", QStringList());
//	accounts << account->id();
//	cfg.setValue("accounts", accounts);
}

void QuetzalProtocol::loadAccounts()
{
	initActions();
	GList *accounts = purple_accounts_get_all();
	for (; accounts; accounts = g_list_next(accounts)) {
		PurpleAccount *purpleAccount = reinterpret_cast<PurpleAccount*>(accounts->data);
		const char *protocolId = purple_account_get_protocol_id(purpleAccount);
		if (qstrcmp(m_plugin->info->id, protocolId))
			continue;
		QuetzalAccount *account = new QuetzalAccount(purpleAccount, this);
		registerAccount(account);
	}
	
//	QStringList accounts = config("general").value("accounts", QStringList());
//	debug() << id() << accounts;
//	foreach(const QString &id, accounts)
//		registerAccount(new QuetzalAccount(id, this));
}

void QuetzalProtocol::onAccountRemoved(QObject *object)
{
	QuetzalAccount *account = static_cast<QuetzalAccount*>(object);
	m_accounts.remove(m_accounts.key(account));
}

void QuetzalProtocol::registerAccount(QuetzalAccount *account)
{
	m_accounts.insert(account->id(), account);
	connect(account, SIGNAL(destroyed(QObject*)), this, SLOT(onAccountRemoved(QObject*)));
	emit accountCreated(account);
}


QuetzalProtocolHook::QuetzalProtocolHook(const QuetzalMetaObject *meta, PurplePlugin *plugin)
    : QuetzalProtocol(plugin), m_metaObject(meta)
{
}

QuetzalProtocolHook::~QuetzalProtocolHook()
{
}

const QMetaObject *QuetzalProtocolHook::metaObject() const
{
	return m_metaObject;
}

void *QuetzalProtocolHook::qt_metacast(const char *_clname)
{
	if (!_clname) return 0;
	if (!qstrcmp(_clname, m_metaObject->className()))
		return static_cast<void*>(this);
	return QuetzalProtocolHook::qt_metacast(_clname);
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

