/****************************************************************************
 *  quetzalaccount.cpp
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

#include "quetzalaccount.h"
#include "quetzalprotocol.h"
#include "quetzalblist.h"
#include "quetzaljoinchatdialog.h"
#include <qutim/passworddialog.h>
#include <qutim/debug.h>
#include <qutim/contactlist.h>
#include <QFile>
#include <qutim/json.h>

extern Status quetzal_get_status(PurplePresence *presence);
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

QuetzalAccount::QuetzalAccount(const QString &id, QuetzalProtocol *protocol) :
	Account(id, protocol)
{
	m_account = purple_account_new(id.toUtf8(), protocol->plugin()->info->id);
	m_account->ui_data = this;
	if (PURPLE_PLUGIN_PROTOCOL_INFO(protocol->plugin())->chat_info != NULL) {
		addAction((new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Quetzal", "Join groupchat"), this, SLOT(showJoinGroupChat())))->setType(1));
	}
	debug() << "created!" << this << m_account->protocol_id;
	for (GList *it = m_account->status_types; it; it = it->next) {
		PurpleStatusType *type = (PurpleStatusType *)it->data;
		debug() << purple_status_type_get_id(type) << purple_status_type_get_name(type);
		for (GList *jt = purple_status_type_get_attrs(type); jt; jt = jt->next) {
			PurpleStatusAttr *attr = (PurpleStatusAttr *)jt->data;
			debug() << "!" << purple_status_attr_get_id(attr) << purple_status_attr_get_name(attr)
					<< purple_status_attr_get_value(attr);
		}
	}
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


	m_is_loading = true;
	load(config());
	m_is_loading = false;
//	if (protocol->id() == "jabber" && id.contains("qutim.org")) {
	if (!purple_account_get_enabled(m_account, "qutim"))
		purple_account_set_enabled(m_account, "qutim", TRUE);

//		purple_account_connect(m_account);
//	} else
//		purple_account_set_enabled(m_account, "qutim", FALSE);
}

ChatUnit *QuetzalAccount::getUnit(const QString &unitId, bool create)
{
	return m_contacts.value(unitId);
}

void QuetzalAccount::createNode(PurpleBlistNode *node)
{
	if (m_is_loading)
		return;
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		PurpleBuddy *buddy = PURPLE_BUDDY(node);
		if (m_contacts.contains(buddy->name))
			return;
		QuetzalContact *contact = new QuetzalContact(buddy);
		m_contacts.insert(contact->id(), contact);
		ContactList::instance()->addContact(contact);
	}
}

void QuetzalAccount::load(Config cfg)
{
	ConfigGroup group = cfg.group("contactlist");
	QByteArray general = tr("General").toUtf8();
	foreach (const QString &id, group.groupList()) {
		ConfigGroup contact = group.group(id);
		QString name = contact.value("name", QString());
		PurpleContact *pc = purple_contact_new();
		PurpleBuddy *buddy = purple_buddy_new(m_account,
											  id.toUtf8().constData(),
											  name.toUtf8().constData());
		buddy->node.parent = PURPLE_BLIST_NODE(pc);
		QuetzalContact *qc = new QuetzalContact(buddy);
		qc->m_tags = contact.value("tags", QStringList()).toSet();
		QByteArray tag;
		if (qc->m_tags.isEmpty()) {
			tag = general;
		} else {
			tag = qc->m_tags.begin()->toUtf8();
		}
		PurpleGroup *group = purple_group_new(tag.constData());
		buddy->node.parent->parent = PURPLE_BLIST_NODE(group);
		ContactList::instance()->addContact(qc);
	}
	ConfigGroup bookmarks = cfg.group("bookmarks");
	foreach (const QString &name, bookmarks.groupList()) {
		ConfigGroup chat = bookmarks.group(name);
		GHashTable *table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		foreach (const QString &id, chat.groupList()) {
			g_hash_table_insert(table,
								g_strdup(id.toUtf8().constData()),
								g_strdup(chat.value(id, QString()).toUtf8().constData()));
		}
		PurpleChat *pc = purple_chat_new(m_account, name.toUtf8().constData(), table);
		purple_blist_add_chat(pc, NULL, NULL);
	}
}

static void quetzal_save_chat(const char *key, const char *value, ConfigGroup *group)
{
	group->setValue(key, QString(value));
}

void QuetzalAccount::save()
{
	Config cfg = config();
	ConfigGroup group = cfg.group("contactlist");
	QHash<QString, QuetzalContact *>::iterator it = m_contacts.begin();
	for (; it != m_contacts.end(); it++) {
		it.value()->save(group.group(it.value()->id()));
	}
	ConfigGroup bookmarks = cfg.group("bookmarks");
	GList *chats = quetzal_blist_get_chats(m_account);
	for (GList *it = chats; it; it = it->next) {
		PurpleChat *chat = PURPLE_CHAT(it->data);
		ConfigGroup group = bookmarks.group(chat->alias);
		g_hash_table_foreach(chat->components, (GHFunc)quetzal_save_chat, &group);
	}
	g_list_free(chats);
	group.sync();
}

void QuetzalAccount::save(QuetzalContact *contact)
{
	ConfigGroup group = config("contactlist");
	contact->save(group.group(contact->id()));
	group.sync();
}

void QuetzalAccount::remove(QuetzalContact *contact)
{
	ConfigGroup group = config("contactlist");
	group.removeGroup(contact->id());
	group.sync();
	m_contacts.remove(contact->id());
	ContactList::instance()->removeContact(contact);
	contact->deleteLater();
}

void QuetzalAccount::save(PurpleChat *chat)
{
	debug() << Q_FUNC_INFO << chat->alias;
	ConfigGroup bookmarks = config("bookmarks");
	ConfigGroup group = bookmarks.group(chat->alias);
	g_hash_table_foreach(chat->components, (GHFunc)quetzal_save_chat, &group);
	bookmarks.sync();
}

void QuetzalAccount::remove(PurpleChat *chat)
{
	debug() << Q_FUNC_INFO << chat->alias;
	ConfigGroup bookmarks = config("bookmarks");
	bookmarks.removeGroup(chat->alias);
	bookmarks.sync();
}

void QuetzalAccount::addChatUnit(ChatUnit *unit)
{
	m_units.insert(unit->id(), unit);
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

extern QVariant quetzal_value2variant(const PurpleValue *value);

extern Status quetzal_get_status(PurpleStatus *status, const QString &proto);

void QuetzalAccount::setStatusChanged(PurpleStatus *status)
{
	Status stat = quetzal_get_status(status, protocol()->id());
	Account::setStatus(stat);
	statusChanged(stat);
}

struct QuetzalAccountPasswordInfo
{
	PurpleRequestFieldsCb okCb;
	PurpleRequestFieldsCb cancelCb;
	void *userData;
};

Q_DECLARE_METATYPE(QuetzalAccountPasswordInfo)

void quetzal_password_build(PurpleRequestFieldsCb cb, const char *password,
							bool remember, void *userData)
{
	PurpleRequestFields *fields = purple_request_fields_new();
	PurpleRequestFieldGroup *group = purple_request_field_group_new(NULL);
	purple_request_fields_add_group(fields, group);
	PurpleRequestField *field;
	field = purple_request_field_string_new("password", "", password, FALSE);
	purple_request_field_group_add_field(group, field);
	field = purple_request_field_bool_new("remember", "", remember);
	purple_request_field_group_add_field(group, field);
	cb(userData, fields);
	purple_request_fields_destroy(fields);
}

void QuetzalAccount::requestPassword(PurpleRequestFieldsCb okCb, PurpleRequestFieldsCb cancelCb,
									 void *userData)
{
	QByteArray password = config("general").value("passwd", QString(), Config::Crypted).toUtf8();
	if (password != purple_account_get_password(m_account)) {
		quetzal_password_build(okCb, password.constData(), false, userData);
		return;
	}

	PasswordDialog *dialog = PasswordDialog::request(this);
	QuetzalAccountPasswordInfo info = {okCb, cancelCb, userData};
	dialog->setProperty("info", qVariantFromValue(info));
	connect(dialog, SIGNAL(entered(QString,bool)), this, SLOT(onPasswordEntered(QString,bool)));
	connect(dialog, SIGNAL(rejected()), this, SLOT(onPasswordRejected()));
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

void QuetzalAccount::onPasswordEntered(const QString &password, bool remember)
{
	PasswordDialog *dialog = qobject_cast<PasswordDialog *>(sender());
	QuetzalAccountPasswordInfo info = dialog->property("info").value<QuetzalAccountPasswordInfo>();
	quetzal_password_build(info.okCb, password.toUtf8().constData(),
						   false, info.userData);
	if (remember)
		config("general").setValue("passwd", password, Config::Crypted);
	dialog->deleteLater();
}

void QuetzalAccount::onPasswordRejected()
{
	PasswordDialog *dialog = qobject_cast<PasswordDialog *>(sender());
	QuetzalAccountPasswordInfo info = dialog->property("info").value<QuetzalAccountPasswordInfo>();
	quetzal_password_build(info.okCb, "", false, info.userData);
	dialog->deleteLater();
}

void QuetzalAccount::showJoinGroupChat()
{
	QDialog *dialog = new QuetzalJoinChatDialog(m_account->gc);
	dialog->show();
}

Q_EXTERN_C Q_DECL_EXPORT void Q_STANDARD_CALL
purple_account_request_password(PurpleAccount *account, GCallback ok_cb,
							   GCallback cancel_cb, void *user_data)
{
	QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(account ? account->ui_data : 0);
	if (!acc)
		return;
	acc->requestPassword(reinterpret_cast<PurpleRequestFieldsCb>(ok_cb),
						 reinterpret_cast<PurpleRequestFieldsCb>(cancel_cb), user_data);
}
