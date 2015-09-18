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

// from Qt5's qmetaobjectbuilder.cpp
static void writeString(char *out, int i, const QByteArray &str,
		const int offsetOfStringdataMember, int &stringdataOffset)
{
	int size = str.size();
	qptrdiff offset = offsetOfStringdataMember + stringdataOffset
		- i * sizeof(QByteArrayData);
	const QByteArrayData data =
		Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(size, offset);
	memcpy(out + i * sizeof(QByteArrayData), &data, sizeof(QByteArrayData));
	memcpy(out + offsetOfStringdataMember + stringdataOffset, str.constData(), size);
	out[offsetOfStringdataMember + stringdataOffset + size] = '\0';
	stringdataOffset += size + 1;
}

/**
 * WARNING: MAGIC.
 * Here we are trying to be QMetaObjectBuilder.
 * So if you just don't get any idea what the heck is happening here
 * read more about "dynamic meta object" problem
 * also about structure of QMetaObject description
 * (because all code below just replicate current structure of qmetaobject)
 * You can also use "git log" to see changes of this file between qt4 and qt5
 * \see https://bugs.kde.org/show_bug.cgi?id=343779
 * \see https://quickgit.kde.org/?p=kross.git&a=blobdiff&h=48a993472a1606700adde87a46233da5204b5876&hp=7b8f46bc24fc5954abc1c4ff51a2e32d2380ad05&hb=f079cc63c0fb217dffe57ec300c94facc4ed1886&f=src%2Fcore%2Fmetafunction.h
 * \see http://habrahabr.ru/post/258441/
 */
QuetzalMetaObject::QuetzalMetaObject(PurplePlugin *protocol)
{
	uint *data = (uint*) calloc(17, sizeof(uint));

	data[ 0] = 7;  // revision
	data[ 1] = 0;  // classname (the first string)

	data[ 2] = 1;  // classinfo count
	data[ 3] = 14; // classinfo data

	data[ 4] = 0;  // methods count
	data[ 5] = 0;  // methods data

	data[ 6] = 0;  // properties count
	data[ 7] = 0;  // properties data

	data[ 8] = 0;  // enums/sets count
	data[ 9] = 0;  // enums/sets data

	data[10] = 0;  // constructors count
	data[11] = 0;  // constructors data

	data[12] = 0;  // flags

	data[13] = 0;  // signal count

	QByteArray className("Quetzal::Protocol::");
	className += protocol->info->id;
	className.replace('-', '_');
	const QByteArray keyStr("Protocol");
	const QByteArray valueStr(quetzal_fix_protocol_name(protocol->info->name));
	// because we have ONE classname, ONE classInfoName and ONE classInfoValue
	int offsetOfStringdataMember = 3 * sizeof(QByteArrayData);
	int stringdataOffset = 0;
	char* stringData = new char[offsetOfStringdataMember + className.size() + 1 + keyStr.size() + 1 + valueStr.size() + 1];
	writeString(stringData, /*index*/0, className, offsetOfStringdataMember, stringdataOffset);
	writeString(stringData, 1, keyStr, offsetOfStringdataMember, stringdataOffset);
	writeString(stringData, 2, valueStr, offsetOfStringdataMember, stringdataOffset);

	data[14] = 1; // because 0 is ClassName
	data[15] = 2; //

	data[16] = 0; // eods

	d.superdata = &QuetzalProtocol::staticMetaObject;
	d.stringdata = reinterpret_cast<const QByteArrayData*>(stringData);
	d.data = data;
	d.relatedMetaObjects = 0;
	d.extradata = 0;
}

QuetzalMetaObject::QuetzalMetaObject(QuetzalProtocolGenerator *protocol)
{
	uint *data = (uint*) calloc(17, sizeof(uint));

	data[ 0] = 7;  // revision
	data[ 1] = 0;  // classname (the first string)

	data[ 2] = 1;  // classinfo count
	data[ 3] = 14; // classinfo data

	data[ 4] = 0;  // methods count
	data[ 5] = 0;  // methods data

	data[ 6] = 0;  // properties count
	data[ 7] = 0;  // properties data

	data[ 8] = 0;  // enums/sets count
	data[ 9] = 0;  // enums/sets data

	data[10] = 0;  // constructors count
	data[11] = 0;  // constructors data

	data[12] = 0;  // flags

	data[13] = 0;  // signal count

	const QMetaObject *meta = protocol->metaObject();

	QByteArray className("Quetzal::AccountWizard::");
	className += protocol->plugin()->info->id;
	className.replace('-', '_');
	const QByteArray keyStr("DependsOn");
	const QByteArray valueStr(meta->className());

	// because we have ONE classname, ONE classInfoName and ONE classInfoValue
	int offsetOfStringdataMember = 3 * sizeof(QByteArrayData);
	int stringdataOffset = 0;
	char* stringData = new char[offsetOfStringdataMember + className.size() + 1 + keyStr.size() + 1 + valueStr.size() + 1];
	writeString(stringData, /*index*/0, className, offsetOfStringdataMember, stringdataOffset);
	writeString(stringData, 1, keyStr, offsetOfStringdataMember, stringdataOffset);
	writeString(stringData, 2, valueStr, offsetOfStringdataMember, stringdataOffset);

	data[14] = 1; // key
	data[15] = 2; // value

	data[16] = 0; // eods

	d.superdata = &QuetzalAccountWizard::staticMetaObject;
	d.stringdata = reinterpret_cast<const QByteArrayData*>(stringData);
	d.data = data;
	d.relatedMetaObjects = 0;
	d.extradata = 0;
}
