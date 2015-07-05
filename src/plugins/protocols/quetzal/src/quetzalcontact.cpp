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

#include "quetzalcontact.h"
#include "quetzalaccount.h"
#include "quetzalactiongenerator.h"
#include <qutim/debug.h>
#include <qutim/message.h>
#include <qutim/tooltip.h>
#include <qutim/chatsession.h>
#include <qutim/notification.h>

QVariant quetzal_value2variant(const PurpleValue *value)
{
	switch (purple_value_get_type(value)) {
	default:
	case PURPLE_TYPE_UNKNOWN:
		return QVariant(QVariant::Invalid);
	case PURPLE_TYPE_SUBTYPE:
		return QVariant(QVariant::Invalid);
	case PURPLE_TYPE_CHAR:
		return QVariant(value->data.char_data);
	case PURPLE_TYPE_UCHAR:
		return QVariant(value->data.uchar_data);
	case PURPLE_TYPE_BOOLEAN:
		return QVariant(bool(value->data.boolean_data));
	case PURPLE_TYPE_SHORT:
		return QVariant(value->data.short_data);
	case PURPLE_TYPE_USHORT:
		return QVariant(value->data.ushort_data);
	case PURPLE_TYPE_INT:
		return QVariant(value->data.int_data);
	case PURPLE_TYPE_UINT:
		return QVariant(value->data.uint_data);
	case PURPLE_TYPE_LONG:
		if (sizeof(long) == sizeof(int))
			return QVariant(int(value->data.long_data));
		else
			return QVariant(qlonglong(value->data.long_data));
	case PURPLE_TYPE_ULONG:
		if (sizeof(ulong) == sizeof(uint))
			return QVariant(uint(value->data.ulong_data));
		else
			return QVariant(qulonglong(value->data.ulong_data));
	case PURPLE_TYPE_INT64:
		return QVariant(qlonglong(value->data.int64_data));
	case PURPLE_TYPE_UINT64:
		return QVariant(qulonglong(value->data.uint64_data));
	case PURPLE_TYPE_STRING:
		return QVariant(QString::fromUtf8(value->data.string_data));
	case PURPLE_TYPE_OBJECT:
	case PURPLE_TYPE_POINTER:
		return QVariant(QVariant::Invalid);
	case PURPLE_TYPE_ENUM:
		return QVariant(value->data.enum_data);
	case PURPLE_TYPE_BOXED:
		return QVariant(QVariant::Invalid);
	}
}

Status quetzal_get_status(PurpleStatusType *status_type, const QString &proto)
{
	const char *status_id = purple_status_type_get_id(status_type);
	PurpleStatusPrimitive primitive = purple_status_type_get_primitive(status_type);
	Status::Type type;
	switch (primitive) {
	case PURPLE_STATUS_UNSET:
		type = Status::Offline;
		break;
	case PURPLE_STATUS_OFFLINE:
		type = Status::Offline;
		break;
	case PURPLE_STATUS_AVAILABLE:
		if (g_str_equal(status_id, "freeforchat")
			|| g_str_equal(status_id, "free4chat"))
			type = Status::FreeChat;
		else
			type = Status::Online;
		break;
	case PURPLE_STATUS_UNAVAILABLE:
		if (g_str_equal(status_id, "dnd"))
			type = Status::DND;
//		else if (g_str_equal(status_id, "occupied"))
//			type = Status::
		else
			type = Status::NA;
		break;
	case PURPLE_STATUS_INVISIBLE:
		type = Status::Invisible;
		break;
	case PURPLE_STATUS_AWAY:
		type = Status::Away;
		break;
	case PURPLE_STATUS_EXTENDED_AWAY:
		type = Status::Away;
		break;
	case PURPLE_STATUS_MOBILE:
	case PURPLE_STATUS_TUNE:
	default:
		type = Status::Online;
		break;
	}
//	FIXME
//	return Status::instance(type, proto.toLatin1().constData());
	Status status(type);
	status.setName(purple_status_type_get_name(status_type));
	status.initIcon(proto);
	return status;
}

Status quetzal_get_status(PurpleStatus *status, const QString &proto)
{
	PurpleStatusType *status_type = purple_status_get_type(status);
	Status qStatus = quetzal_get_status(status_type, proto);

	for (GList *it = purple_status_type_get_attrs(status_type); it; it = it->next) {
		PurpleStatusAttr *attr = (PurpleStatusAttr *)it->data;
		const char *id = purple_status_attr_get_id(attr);
//		QString name = purple_status_attr_get_name(attr);
		QVariant value = quetzal_value2variant(purple_status_attr_get_value(attr));
//		debug() << "!" << type << id /*<< name */<< value;
		if (g_str_equal(id, "message"))
			qStatus.setText(value.toString());
		else
			qStatus.setProperty(id, value);
	}
	return qStatus;
}

Status quetzal_get_status(PurplePresence *presence)
{
	PurpleStatus *status = purple_presence_get_active_status(presence);

	PurpleAccount *account = purple_presence_get_account(presence);
	PurplePlugin *proto = account ? purple_plugins_find_with_id(account->protocol_id) : 0;
	QString proto_id;
	if (proto) {
		proto_id = QString::fromLatin1(proto->info->name).toLower();
	}

	return quetzal_get_status(status, proto_id);
}


QuetzalContact::QuetzalContact(PurpleBuddy *buddy) :
	Contact(reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data))
{
//	PurpleBlistNode *node = &buddy->node;
	m_id = buddy->name;
	m_name = purple_buddy_get_alias(buddy);
	m_status = quetzal_get_status(buddy->presence);
	addBuddy(buddy);
//	ensureAvatarPath();
//	while (!!(node = node->parent)) {
//		if (PURPLE_BLIST_NODE_IS_CONTACT(node)) {
//			node->ui_data = this;
//		} else if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
//			PurpleGroup *group = PURPLE_GROUP(node);
//			m_tags.append(group->name);
//		}
//	}
//	m_tags.removeDuplicates();
}

void QuetzalContact::save(ConfigGroup group)
{
	group.setValue("name", m_name);
	group.setValue("tags", m_tags);
}

void QuetzalContact::update(PurpleBuddy *buddy)
{
//	QStringList tags_;
//	PurpleBlistNode *node = &m_buddy->node;
//	while (!!(node = node->parent)) {
//		if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
//			PurpleGroup *group = PURPLE_GROUP(node);
//			tags_.append(group->name);
//		}
//	}
//	tags_.removeDuplicates();
//	if (m_tags != tags_) {
//		QStringList previous = m_tags;
//		m_tags = tags_;
//		tagsChanged(m_tags, previous);
//	}
	QString name = purple_buddy_get_alias(buddy);
	if (name != m_name) {
		QString previous = m_name;
		m_name = name;
		emit nameChanged(m_name, previous);
	}
	bool isConnected = buddy->account->gc && buddy->account->gc->state == PURPLE_CONNECTED;
	Status status = isConnected ? quetzal_get_status(buddy->presence) : Status(Status::Offline);
//	qDebug() << Q_FUNC_INFO << status << purple_status_type_get_id(purple_status_get_type(purple_presence_get_active_status(buddy->presence)));
//	debug() << Q_FUNC_INFO << buddy->name << m_status << status;
	if (m_status.type() != status.type()
		|| m_status.subtype() != status.subtype()
		|| m_status.text() != status.text()) {
//		debug() << Q_FUNC_INFO << m_id << status;
		Status previous = m_status;
		m_status = status;
		NotificationRequest request(this, status, previous);
		request.send();
		emit statusChanged(m_status, previous);
	}
	ensureAvatarPath();
}

int QuetzalContact::addBuddy(PurpleBuddy *buddy)
{
	buddy->node.ui_data = this;
	m_buddies << buddy;
	QStringList oldTags = m_tags;
	if (PurpleGroup *group = purple_buddy_get_group(buddy))
		m_tags.append(group->name);
	emit tagsChanged(m_tags, oldTags);
	return m_buddies.size();
}

int QuetzalContact::removeBuddy(PurpleBuddy *buddy)
{
	m_buddies.removeOne(buddy);
	QStringList oldTags = m_tags;
	if (PurpleGroup *group = purple_buddy_get_group(buddy))
		m_tags.removeOne(group->name);
	emit tagsChanged(m_tags, oldTags);
	return m_buddies.size();
}

QString QuetzalContact::avatar() const
{
	return m_avatarPath;
}

void QuetzalContact::ensureAvatarPath()
{
	QString path;
	PurpleBuddy *buddy = m_buddies.first();
	PurpleBuddyIcon *icon = purple_buddy_icons_find(buddy->account, buddy->name);
	if (icon) {
		char *str = purple_buddy_icon_get_full_path(icon);
		if (str) {
			path = str;
			g_free(str);
		}
	}
	if (m_avatarPath != path) {
		m_avatarPath = path;
		emit avatarChanged(m_avatarPath);
	}
}

QString QuetzalContact::id() const
{
	return m_id;
}

QString QuetzalContact::name() const
{
	return m_name;
}

QStringList QuetzalContact::tags() const
{
	return m_tags;
}

Status QuetzalContact::status() const
{
	return m_status;
}

bool quetzal_send_message(PurpleAccount *account, Buddy *unit, const Message &message)
{
	if (!account->gc)
		return false;
	QByteArray name = unit->id().toUtf8().constData();
	PurpleConversation *conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM, name, account);
	if (!conv) {
		if (ChatLayer::get(unit, false)) {
			conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, account, name);
		} else {
			PurplePluginProtocolInfo *prpl = PURPLE_PLUGIN_PROTOCOL_INFO(account->gc->prpl);
			int result = prpl->send_im(account->gc, name,
									   message.text().toUtf8().constData(), static_cast<PurpleMessageFlags>(0));
			return result > 0;
		}
	}
	purple_conv_im_send(PURPLE_CONV_IM(conv), message.text().toUtf8().constData());
	return true;
}

bool QuetzalContact::sendMessage(const Message &message)
{
	PurpleBuddy *buddy = m_buddies.first();
	return quetzal_send_message(buddy->account, this, message);
}

void QuetzalContact::setName(const QString &name)
{
	PurpleBuddy *buddy = m_buddies.first();
	purple_blist_alias_buddy(buddy, name.toUtf8().constData());
	serv_alias_buddy(buddy);
}

extern PurpleBlistNode *quetzal_blist_get_last_sibling(PurpleBlistNode *node);
extern PurpleBlistNode *quetzal_blist_get_last_child(PurpleBlistNode *node);

void QuetzalContact::setTags(const QStringList &tags)
{
	PurpleBuddy *original = purple();
	GList *buddies = NULL;
	GList *groups = NULL;
	for (int i = 0; i < tags.size(); i++) {
		if (m_tags.contains(tags.at(i)))
			continue;
		QByteArray groupName = tags.at(i).toUtf8();
		PurpleGroup *pg = purple_find_group(groupName.constData());
		if (!pg) {
			pg = purple_group_new(groupName.constData());
			purple_blist_add_group(pg, quetzal_blist_get_last_sibling(purple_blist_get_root()));
		}
		PurpleContact *pc = purple_contact_new();
		purple_blist_add_contact(pc, pg, quetzal_blist_get_last_child(PURPLE_BLIST_NODE(pg)));
		PurpleBuddy *buddy = purple_buddy_new(original->account, original->name, original->alias);
		purple_blist_add_buddy(buddy, pc, pg, quetzal_blist_get_last_child(PURPLE_BLIST_NODE(pc)));
		buddies = g_list_append(buddies, buddy);
	}
	purple_account_add_buddies(original->account, buddies);
	g_list_free(buddies);
	buddies = NULL;
	groups = NULL;
	for (int i = 0; i < m_buddies.size(); i++) {
		if (!tags.contains(m_tags.at(i))) {
			buddies = g_list_append(buddies, m_buddies.at(i));
			groups = g_list_append(groups, purple_buddy_get_group(m_buddies.at(i)));
		}
	}
	purple_account_remove_buddies(original->account, buddies, groups);
	g_list_free(buddies);
	g_list_free(groups);
}

bool QuetzalContact::isInList() const
{
	return true;
}

void QuetzalContact::setInList(bool inList)
{
	Q_UNUSED(inList);
}

void quetzal_menu_add(MenuController::ActionList &actions, void *node,
					  GList *menu, const QList<QByteArray> &off, int type)
{
	int i = 0;
	for (GList *it = menu; it; it = it->next, i--) {
		PurpleMenuAction *action = (PurpleMenuAction *)it->data;
		ActionGenerator *gen = new QuetzalActionGenerator(action, node);
		gen->setType(type)->setPriority(i);
		actions << (MenuController::Action){ gen, off };
		QList<QByteArray> offs = off;
		offs.append(action->label);
		quetzal_menu_add(actions, node, action->children, offs, ActionGenerator::GeneralType);
		g_list_free(action->children);
		purple_menu_action_free(action);
	}
}

MenuController::ActionList QuetzalContact::dynamicActions() const
{
	MenuController::ActionList actions;
	PurpleBuddy *buddy = m_buddies.value(0);
	if (!buddy || !buddy->account->gc)
		return actions;
	PurpleBlistNode *node = const_cast<PurpleBlistNode *>(&buddy->node);
	GList *menu = NULL;
	PurplePluginProtocolInfo *prpl = PURPLE_PLUGIN_PROTOCOL_INFO(buddy->account->gc->prpl);
	if (PURPLE_PROTOCOL_PLUGIN_HAS_FUNC(prpl, blist_node_menu)) {
		menu = prpl->blist_node_menu(node);
		quetzal_menu_add(actions, node, menu, QList<QByteArray>(), 2);
		g_list_free(menu);
	}
	menu = purple_blist_node_get_extended_menu(node);
	quetzal_menu_add(actions, node, menu, QList<QByteArray>(), 3);
	g_list_free(menu);
	return actions;
}

bool QuetzalContact::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		PurpleBuddy *buddy = m_buddies.first();
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		if (!buddy->account->gc)
			return Contact::event(ev);

		PurplePluginProtocolInfo *prpl = PURPLE_PLUGIN_PROTOCOL_INFO(buddy->account->gc->prpl);
		if (prpl->tooltip_text) {
			PurpleNotifyUserInfo *user_info = purple_notify_user_info_new();
			prpl->tooltip_text(PURPLE_BUDDY(buddy), user_info, true);
			GList *it = purple_notify_user_info_get_entries(user_info);
			for (; it; it = it->next) {
				PurpleNotifyUserInfoEntry *entry =
						reinterpret_cast<PurpleNotifyUserInfoEntry *>(it->data);
				LocalizedString label = purple_notify_user_info_entry_get_label(entry);
				QString data = purple_notify_user_info_entry_get_value(entry);
				event->addField(label, data);
			}
			purple_notify_user_info_destroy(user_info);
		}
	}
	return Contact::event(ev);
}

