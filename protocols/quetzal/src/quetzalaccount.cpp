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

#include "quetzalaccount.h"
#include "quetzalprotocol.h"
#include "quetzalblist.h"
#include "quetzaljoinchatdialog.h"
#include "quetzalchatuser.h"
#include <qutim/passworddialog.h>
#include <qutim/debug.h>
#include <qutim/conference.h>
#include <QFile>
#include <qutim/json.h>
#include "quetzalactiongenerator.h"

extern "C" {
	void _purple_buddy_icons_blist_loaded_cb();
}

extern Status quetzal_get_status(PurplePresence *presence);
extern QVariant quetzal_value2variant(const PurpleValue *value);
static PurpleStatus *quetzal_get_correct_status(PurplePresence *presence, Status qutim_status)
{
	PurpleStatus *status = NULL;

	PurpleStatusPrimitive type;
	switch (qutim_status.type()) {
	case Status::Online:
		type = PURPLE_STATUS_AVAILABLE;
		break;
	case Status::FreeChat:
		if ((status = purple_presence_get_status(presence, "freeforchat"))
			|| (status = purple_presence_get_status(presence, "free4chat")))
			return status;
		type = PURPLE_STATUS_AVAILABLE;
		break;
	case Status::Away:
		type = PURPLE_STATUS_AWAY;
		break;
	case Status::NA:
		type = PURPLE_STATUS_EXTENDED_AWAY;
		break;
	case Status::DND:
		if (!!(status = purple_presence_get_status(presence, "dnd")))
			return status;
		type = PURPLE_STATUS_UNAVAILABLE;
		break;
	case Status::Invisible:
		type = PURPLE_STATUS_INVISIBLE;
		break;
	case Status::Offline:
		type = PURPLE_STATUS_OFFLINE;
		break;
	default:
		type = PURPLE_STATUS_UNSET;
		break;
	}

	return purple_presence_get_status(presence, purple_primitive_get_id_from_type(type));
}

PurpleBlistNode *quetzal_blist_get_last_sibling(PurpleBlistNode *node)
{
	if (!node)
		return NULL;
	while (node->next)
		node = node->next;
	return node;
}

PurpleBlistNode *quetzal_blist_get_last_child(PurpleBlistNode *node)
{
	if (!node)
		return NULL;
	return quetzal_blist_get_last_sibling(node->child);
}

QuetzalContactsFactory::QuetzalContactsFactory(QuetzalAccount *account) : m_account(account)
{
}

Contact *QuetzalContactsFactory::addContact(const QString &id, const QVariantMap &data)
{
	QByteArray group = data.value(QLatin1String("group")).toString().toUtf8();
	if (group.isEmpty()) {
		// For compatibility with other implementations
		// Looks like libpurple doesn't support multiple groups
		// Or does it at some unknown way
		group = data.value(QLatin1String("tags")).toStringList().value(0).toUtf8();
	}
	QString name = data.value(QLatin1String("name")).toString();
	
	const char *groupName = group.isEmpty() ? "Buddies" : group.constData();
	PurpleGroup *pg = purple_find_group(groupName);
	if (!pg) {
		pg = purple_group_new(groupName);
		purple_blist_add_group(pg, quetzal_blist_get_last_sibling(purple_blist_get_root()));
	}
	PurpleContact *pc = purple_contact_new();
	purple_blist_add_contact(pc, pg, quetzal_blist_get_last_child(PURPLE_BLIST_NODE(pg)));
	PurpleBuddy *buddy = purple_buddy_new(m_account->purple(), id.toUtf8().constData(),
										  name.toUtf8().constData());
	PurpleBlistNode *node = PURPLE_BLIST_NODE(buddy);
	purple_blist_add_buddy(buddy, pc, pg, quetzal_blist_get_last_child(PURPLE_BLIST_NODE(pc)));
	QMapIterator<QString, QVariant> it(data.value(QLatin1String("quetzal_settings")).toMap());
	while (it.hasNext()) {
		it.next();
		QByteArray key = it.key().toUtf8();
		QVariant::Type type = it.value().type();
		if (type == QVariant::Bool) {
			purple_blist_node_set_bool(node, key.constData(), it.value().toBool());
		} else if (type == QVariant::Int || type == QVariant::UInt
				 || type == QVariant::LongLong || type == QVariant::ULongLong) {
			purple_blist_node_set_int(node, key.constData(), it.value().toInt());
		} else {
			purple_blist_node_set_string(node, key.constData(), it.value().toString().toUtf8());
		}
	}
	QuetzalContact *contact = reinterpret_cast<QuetzalContact*>(node->ui_data);
	contact->update(buddy);
	return contact;
}

void quetzal_serialize_node(const char *key, PurpleValue *value, QVariantMap *data)
{
	data->insert(key, quetzal_value2variant(value));
}

void QuetzalContactsFactory::serialize(Contact *contact, QVariantMap &data)
{
	QuetzalContact *c = qobject_cast<QuetzalContact*>(contact);
	data.insert(QLatin1String("group"), c->tags().value(0));
	data.insert(QLatin1String("name"), c->name());
	PurpleBuddy *buddy = c->purple();
	QVariantMap settings;
	g_hash_table_foreach(buddy->node.settings, (GHFunc)quetzal_serialize_node, &settings);
	data.insert(QLatin1String("quetzal_settings"), settings);
}

QString quetzal_fix_id(QuetzalProtocol *protocol, const char *username)
{
	/* if (protocol->id() == QLatin1String("irc")) {
	} else */
	if (protocol->id() == QLatin1String("jabber")) {
		QString id = QString::fromUtf8(username);
		return id.section(QLatin1Char('/'), 0, 0);
	}
	return QString::fromUtf8(username);
}

QuetzalAccount::QuetzalAccount(PurpleAccount *account, QuetzalProtocol *protocol) :
		Account(quetzal_fix_id(protocol, account->username), protocol)
{
	m_isLoading = false;
	m_account = account;
	m_account->ui_data = this;
	fillStatusActions();
//	setContactsFactory(new QuetzalContactsFactory(this));
	if (!purple_account_get_enabled(m_account, "qutim"))
		purple_account_set_enabled(m_account, "qutim", TRUE);
//	purple_accounts_add(account);
	save();
}

QuetzalAccount::QuetzalAccount(const QString &id, QuetzalProtocol *protocol) : Account(id, protocol)
{
	Q_ASSERT(!"Shouldn't use this constructor");
	m_isLoading = false;
	Config cfg = config();
	QString purpleId = id;
	if (protocol->id() == QLatin1String("irc")) {
		purpleId = cfg.value(QLatin1String("nicks"), QStringList()).value(0);
		purpleId += QLatin1Char('@');
		int size = cfg.beginArray("servers");
		if (size == 0) {
			critical() << "Invalid irc account without server info:" << id;
			deleteLater();
			return;
		}
		cfg.setArrayIndex(0);
		purpleId += cfg.value(QLatin1String("hostName"), QString());
		cfg.endArray();
	}
	m_account = purple_account_new(purpleId.toUtf8(), protocol->plugin()->info->id);
	m_account->ui_data = this;
	fillStatusActions();
	setContactsFactory(new QuetzalContactsFactory(this));
//	if (PURPLE_PLUGIN_PROTOCOL_INFO(m_account->gc->prpl)->chat_info != NULL) {
//		addAction((new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Quetzal", "Join groupchat"), this, SLOT(showJoinGroupChat())))->setType(1));
//	}
	debug() << "created!" << this << m_account->protocol_id;
	// Hack for anti-auto-connect
	for (GList *it = purple_presence_get_statuses(m_account->presence); it; it = it->next) {
		PurpleStatus *status = reinterpret_cast<PurpleStatus *>(it->data);
//		debug() << purple_status_type_get_primitive(purple_status_get_type(status))
//				<< !!purple_status_is_online(status)
//				<< purple_status_get_id(status)
//				<< purple_status_get_name(status);
		if (!purple_status_is_online(status)) {
			purple_presence_set_status_active(m_account->presence, purple_status_get_id(status), TRUE);
			break;
		}
	}


	m_isLoading = true;
	load(cfg);
	m_isLoading = false;
//	if (protocol->id() == "jabber" && id.contains("qutim.org")) {
	if (!purple_account_get_enabled(m_account, "qutim"))
		purple_account_set_enabled(m_account, "qutim", TRUE);

//		purple_account_connect(m_account);
//	} else
//		purple_account_set_enabled(m_account, "qutim", FALSE);
}

QuetzalAccount::~QuetzalAccount()
{
}

QuetzalContact *QuetzalAccount::createContact(const QString &id)
{
	PurpleContact *pc = purple_contact_new();
	purple_blist_add_contact(pc, NULL, NULL);
	PurpleBuddy *buddy = purple_buddy_new(m_account, id.toUtf8().constData(), 0);
	purple_blist_add_buddy(buddy, pc, purple_contact_get_group(pc), NULL);
	return m_contacts.value(id);
}

void QuetzalAccount::fillStatusActions()
{
//	TODO:
//	for (GList *it = m_account->status_types; it; it = it->next) {
//		PurpleStatusType *type = (PurpleStatusType *)it->data;
//		debug() << purple_status_type_get_id(type) << purple_status_type_get_name(type);
//		for (GList *jt = purple_status_type_get_attrs(type); jt; jt = jt->next) {
//			PurpleStatusAttr *attr = (PurpleStatusAttr *)jt->data;
//			debug() << "!" << purple_status_attr_get_id(attr) << purple_status_attr_get_name(attr)
//					<< purple_status_attr_get_value(attr);
//		}
//	}
}

ChatUnit *QuetzalAccount::getUnitForSession(ChatUnit *unit)
{
	if (QuetzalChatUser *user = qobject_cast<QuetzalChatUser*>(unit)) {
		if (user->purple()->buddy)
			return createContact(user->id());
	}
	return unit;
}

ChatUnit *QuetzalAccount::getUnit(const QString &unitId, bool create)
{
	ChatUnit *unit = m_contacts.value(unitId);
	if (!unit)
		unit = m_units.value(unitId);
	if (!unit && create)
		unit = createContact(unitId);
	return unit;
}

void QuetzalAccount::createNode(PurpleBlistNode *node)
{
	Q_UNUSED(node);
//	qDebug() << Q_FUNC_INFO << m_isLoading;
//	if (m_isLoading)
//		return;
//	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
//		PurpleBuddy *buddy = PURPLE_BUDDY(node);
//		QuetzalContact *contact = m_contacts.value(buddy->name);
//		if (contact) {
//			contact->addBuddy(buddy);
//			return;
//		}
//		contact = new QuetzalContact(buddy);
//		m_contacts.insert(contact->id(), contact);
//		RosterStorage::instance()->addContact(contact);
//		emit contactCreated(contact);
//	}
}

void QuetzalAccount::load(Config cfg)
{
	cfg.beginGroup(QLatin1String("general"));
	QByteArray password = cfg.value(QLatin1String("passwd"), QString(), Config::Crypted).toUtf8();
	if (!password.isEmpty()) {
		purple_account_set_password(m_account, password.constData());
		purple_account_set_remember_password(m_account, true);
	}
	purple_account_set_alias(m_account, cfg.value(QLatin1String("alias"),
												  QString()).toUtf8().constData());
	purple_account_set_user_info(m_account, cfg.value(QLatin1String("userInfo"),
													  QString()).toUtf8().constData());
	QMapIterator<QString, QVariant> it(cfg.value(QLatin1String("quetzal_settings")).toMap());
	while (it.hasNext()) {
		it.next();
		QByteArray key = it.key().toUtf8();
		QVariant::Type type = it.value().type();
		if (type == QVariant::Bool) {
			purple_account_set_bool(m_account, key.constData(), it.value().toBool());
		} else if (type == QVariant::Int || type == QVariant::UInt
				 || type == QVariant::LongLong || type == QVariant::ULongLong) {
			purple_account_set_int(m_account, key.constData(), it.value().toInt());
		} else {
			purple_account_set_string(m_account, key.constData(), it.value().toString().toUtf8());
		}
	}
	cfg.endGroup();
	
	RosterStorage::instance()->load(this);
	
	int size = cfg.beginArray(QLatin1String("chats"));
	QString name = QLatin1String("name");
	QString recent = QLatin1String("recent");
	QString data = QLatin1String("data");
	for (int i = 0; i < size; i++) {
		cfg.setArrayIndex(i);
		QByteArray alias = cfg.value(name, QString()).toUtf8();
		PurpleGroup *group = NULL;
		if (cfg.value(recent, false)) {
			group = purple_find_group("Recent");
			if (!group) {
				group = purple_group_new("Recent");
				purple_blist_add_group(group, NULL);
			}
		}
		GHashTable *comps = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		QMapIterator<QString, QVariant> it(cfg.value(data).toMap());
		while (it.hasNext()) {
			it.next();
			QByteArray key = it.key().toUtf8();
			QByteArray value = it.value().toString().toUtf8();
			g_hash_table_insert(comps, g_strdup(key.constData()), g_strdup(value.constData()));
		}
		PurpleChat *pc = purple_chat_new(m_account, alias.constData(), comps);
		purple_blist_add_chat(pc, NULL, NULL);
	}
	
	_purple_buddy_icons_blist_loaded_cb();
}

static void quetzal_save_chat(const char *key, const char *value, Config *cfg)
{
	cfg->setValue(QString::fromUtf8(key), QString::fromUtf8(value));
}

void QuetzalAccount::timerEvent(QTimerEvent *ev)
{
	if (m_chatTimer.timerId() != ev->timerId())
		return Account::timerEvent(ev);
	m_chatTimer.stop();
	Config cfg = config();
	cfg.remove(QLatin1String("chats"));
	cfg.beginArray(QLatin1String("chats"));
	GList *chats = quetzal_blist_get_chats(m_account);
	int i = 0;
	QString name = QLatin1String("name");
	QString recent = QLatin1String("recent");
	QString data = QLatin1String("data");
	for (GList *it = chats; it; it = it->next, i++) {
		cfg.setArrayIndex(i);
		PurpleChat *chat = PURPLE_CHAT(it->data);
		cfg.setValue(name, QString::fromUtf8(chat->alias));
		cfg.setValue(recent, !qstrcmp(purple_group_get_name(purple_chat_get_group(chat)), "Recent"));
		cfg.beginGroup(data);
		g_hash_table_foreach(chat->components, (GHFunc)quetzal_save_chat, &cfg);
		cfg.endGroup();
	}
	g_list_free(chats);
}

void QuetzalAccount::save()
{
	if (m_isLoading)
		return;
	// Test it
	else
		return;
	Config cfg = config();
	cfg.beginGroup(QLatin1String("general"));
	cfg.setValue(QLatin1String("alias"), purple_account_get_alias(m_account));
	cfg.setValue(QLatin1String("userInfo"), purple_account_get_user_info(m_account));
	if (purple_account_get_remember_password(m_account))
		cfg.setValue(QLatin1String("passwd"), purple_account_get_password(m_account), Config::Crypted);
	else
		cfg.remove(QLatin1String("passwd"));
	QVariantMap settings;
	g_hash_table_foreach(m_account->settings, (GHFunc)quetzal_serialize_node, &settings);
	cfg.setValue(QLatin1String("quetzal_settings"), settings);
}

void QuetzalAccount::save(PurpleBuddy *buddy)
{
	QuetzalContact *contact;
	bool created = false;
	if (!buddy->node.ui_data) {
		QString id = buddy->name;
		contact = m_contacts.value(id);
		if (contact) {
			contact->addBuddy(buddy);
		} else {
			created = true;
			// Ensure loading of avatar from settings
//			Q_UNUSED(purple_buddy_icons_find(m_account, buddy->name));
			contact = m_contacts.insert(id, new QuetzalContact(buddy)).value();
			emit contactCreated(contact);
		}
	} else {
		contact = reinterpret_cast<QuetzalContact*>(buddy->node.ui_data);
	}
	if (!contact)
		return;
	// Test it
	return;
	if (!m_isLoading) {
		if (created)
			RosterStorage::instance()->addContact(contact);
		else
			RosterStorage::instance()->updateContact(contact);
	}
//	ConfigGroup group = config("contactlist");
//	contact->save(group.group(contact->id()));
//	group.sync();
}

void QuetzalAccount::remove(PurpleBuddy *buddy)
{
	QuetzalContact *contact = reinterpret_cast<QuetzalContact*>(buddy->node.ui_data);
	debug() << Q_FUNC_INFO << __LINE__ << contact;
	if (!contact)
		return;
	if (contact->removeBuddy(buddy) == 0) {
		// Test it
		return;
		RosterStorage::instance()->removeContact(contact);
//		FIXME: What should we do?
//		m_contacts.remove(contact->id());
//		contact->deleteLater();
	} else if (!m_isLoading) {
		// Test it
		return;
		RosterStorage::instance()->updateContact(contact);
	}
//	ConfigGroup group = config("contactlist");
//	group.remove(contact->id());
//	group.sync();
}

void QuetzalAccount::save(PurpleChat *chat)
{
	if (!chat->node.ui_data) {
		QuetzalChatGuard::Ptr guard = QuetzalChatGuard::Ptr::create();
		guard->chat = chat;
		chat->node.ui_data = new QuetzalChatGuard::Ptr(guard);
	}
	// Test it
	return;
	if (!m_isLoading && !m_chatTimer.isActive())
		m_chatTimer.start(5000, this);
//	debug() << Q_FUNC_INFO << chat->alias;
//	ConfigGroup bookmarks = config("bookmarks");
//	ConfigGroup group = bookmarks.group(chat->alias);
//	g_hash_table_foreach(chat->components, (GHFunc)quetzal_save_chat, &group);
//	bookmarks.sync();
}

void QuetzalAccount::remove(PurpleChat *chat)
{
	QuetzalChatGuard::Ptr *guard = reinterpret_cast<QuetzalChatGuard::Ptr*>(chat->node.ui_data);
	if (guard) {
		(*guard)->chat = 0;
		delete guard;
	}
	// Test it
	return;
	if (!m_chatTimer.isActive())
		m_chatTimer.start(5000, this);
//	debug() << Q_FUNC_INFO << chat->alias;
//	ConfigGroup bookmarks = config("bookmarks");
//	bookmarks.remove(chat->alias);
//	bookmarks.sync();
}

void QuetzalAccount::addChatUnit(ChatUnit *unit)
{
	m_units.insert(unit->id(), unit);
	debug() << m_units.keys();
	if (Conference *conference = qobject_cast<Conference*>(unit))
		conferenceCreated(conference);
}

void QuetzalAccount::removeChatUnit(ChatUnit *unit)
{
	m_units.remove(unit->id());
}

void QuetzalAccount::setStatus(Status status)
{
	PurpleStatus *purple_status = quetzal_get_correct_status(m_account->presence, status);
	debug() << purple_status_get_id(purple_status) << purple_status_get_name(purple_status);
	purple_presence_set_status_active(m_account->presence, purple_status_get_id(purple_status), TRUE);
	if (status.type() != Status::Offline)
		purple_account_connect(m_account);
}

extern Status quetzal_get_status(PurpleStatus *status, const QString &proto);

void QuetzalAccount::setStatusChanged(PurpleStatus *status)
{
	if (!m_account->gc || m_account->gc->state != PURPLE_CONNECTED)
		return;
	Status stat = quetzal_get_status(status, protocol()->id());
	Account::setStatus(stat);
}

void QuetzalAccount::handleSigningOn()
{
	Status oldStatus = status();
	Account::setStatus(Status(Status::Connecting));
}

void QuetzalAccount::handleSignedOn()
{
	setStatusChanged(purple_account_get_active_status(m_account));
	if (PURPLE_PLUGIN_PROTOCOL_INFO(m_account->gc->prpl)->chat_info)
		resetGroupChatManager(new QuetzalJoinChatManager(this));
}

void QuetzalAccount::handleSignedOff()
{
	Status oldStatus = status();
	Account::setStatus(Status(Status::Offline));
	resetGroupChatManager(0);
	foreach (QuetzalContact *contact, m_contacts) {
		if (contact->purple())
			contact->update(contact->purple());
	}
}

struct QuetzalAccountPasswordInfo
{
	PurpleRequestFields *fields;
	PurpleRequestFieldsCb okCb;
	PurpleRequestFieldsCb cancelCb;
	void *userData;
};

Q_DECLARE_METATYPE(QuetzalAccountPasswordInfo)

QObject *QuetzalAccount::requestPassword(PurpleRequestFields *fields, PurpleRequestFieldsCb okCb,
										 PurpleRequestFieldsCb cancelCb, void *userData)
{
//	QByteArray password = config("general").value("passwd", QString(), Config::Crypted).toUtf8();
//	if (!password.isEmpty()) {
//		PurpleRequestField *passwordField = purple_request_fields_get_field(fields, "password");
//		purple_request_field_string_set_value(passwordField, password.constData());
//	}
//	if (password != purple_account_get_password(m_account)) {
//		quetzal_password_build(okCb, password.constData(), false, userData);
//		return;
//	}
	QuetzalAccountPasswordInfo info = {fields, okCb, cancelCb, userData};
//	QString password = config("general").value("passwd", QString(), Config::Crypted);
//	if (!password.isEmpty()) {
//		fillPassword(info, password);
//		return;
//	}
	PasswordDialog *dialog = PasswordDialog::request(this);
	dialog->setProperty("info", qVariantFromValue(info));
	connect(dialog, SIGNAL(entered(QString,bool)), this, SLOT(onPasswordEntered(QString,bool)));
	connect(dialog, SIGNAL(rejected()), this, SLOT(onPasswordRejected()));
	return dialog;
}

PurpleAccount *QuetzalAccount::purple()
{
	return m_account;
}

int QuetzalAccount::sendRawData(const QByteArray &data)
{
	if (!m_account->gc)
		return -1;

	PurplePluginProtocolInfo *prpl = PURPLE_PLUGIN_PROTOCOL_INFO(m_account->gc->prpl);
	if (PURPLE_PROTOCOL_PLUGIN_HAS_FUNC(prpl, send_raw))
		return prpl->send_raw(m_account->gc, data.constData(), data.length());
	return -1;
}

MenuController::ActionList QuetzalAccount::dynamicActions() const
{
	MenuController::ActionList actions;
	if (!m_account->gc || status() == Status::Connecting)
		return actions;
	GList *menu = m_account->gc->prpl->info->actions(m_account->gc->prpl, m_account->gc);
	if (!menu)
		return actions;
	QList<QByteArray> off;
	int i = 0;
	for (GList *it = menu; it; it = it->next, i--) {
		PurplePluginAction *action = reinterpret_cast<PurplePluginAction*>(it->data);
		action->context = m_account->gc;
		ActionGenerator *gen = new QuetzalActionGenerator(action);
		gen->setType(2)->setPriority(i);
		actions << (MenuController::Action){ gen, off };
	}
	g_list_free(menu);
	return actions;
}

void QuetzalAccount::onPasswordEntered(const QString &password, bool remember)
{
	PasswordDialog *dialog = qobject_cast<PasswordDialog *>(sender());
	QuetzalAccountPasswordInfo info = dialog->property("info").value<QuetzalAccountPasswordInfo>();
	PurpleRequestField *passwordField = purple_request_fields_get_field(info.fields, "password");
	PurpleRequestField *rememberField = purple_request_fields_get_field(info.fields, "remember");
	purple_request_field_string_set_value(passwordField, password.toUtf8().constData());
	purple_request_field_bool_set_value(rememberField, remember);
	info.okCb(info.userData, info.fields);
	purple_request_fields_destroy(info.fields);
	dialog->deleteLater();
//	if (remember) {
//		purple_account_set_password(password);
//	}
//		config("general").setValue("passwd", password, Config::Crypted);
}

void QuetzalAccount::fillPassword(const QuetzalAccountPasswordInfo &info, const QString &password)
{
	PurpleRequestField *passwordField = purple_request_fields_get_field(info.fields, "password");
//	PurpleRequestField *rememberField = purple_request_fields_get_field(info.fields, "remember");
	purple_request_field_string_set_value(passwordField, password.toUtf8().constData());
	info.okCb(info.userData, info.fields);
	purple_request_fields_destroy(info.fields);
}

void QuetzalAccount::onPasswordRejected()
{
	PasswordDialog *dialog = qobject_cast<PasswordDialog *>(sender());
	QuetzalAccountPasswordInfo info = dialog->property("info").value<QuetzalAccountPasswordInfo>();
	info.cancelCb(info.userData, info.fields);
	purple_request_fields_destroy(info.fields);
	dialog->deleteLater();
}

void QuetzalAccount::showJoinGroupChat()
{
//	QDialog *dialog = new QuetzalJoinChatManager(m_account->gc);
//	dialog->show();
}

