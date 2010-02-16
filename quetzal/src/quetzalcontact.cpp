/****************************************************************************
 *  quetzalcontact.cpp
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

#include "quetzalcontact.h"
#include "quetzalaccount.h"
#include "quetzalactiongenerator.h"
#include <qutim/debug.h>
#include <qutim/message.h>

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
	Status status(type);
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
		proto_id = QString::fromUtf8(proto->info->name).toLower();
	}

	return quetzal_get_status(status, proto_id);
}


QuetzalContact::QuetzalContact(PurpleBuddy *buddy) :
	Contact(reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data))
{
	m_buddy = buddy;
	buddy->node.ui_data = this;
	PurpleBlistNode *node = &buddy->node;
	m_id = buddy->name;
	m_name = purple_buddy_get_alias(buddy);
	m_status = quetzal_get_status(m_buddy->presence);
	while (!!(node = node->parent)) {
		if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
			PurpleGroup *group = PURPLE_GROUP(node);
			m_tags.insert(group->name);
		}
	}
}

void QuetzalContact::save(ConfigGroup group)
{
	group.setValue("name", m_name);
	group.setValue("tags", QStringList(m_tags.toList()));
}

void QuetzalContact::update()
{
	QSet<QString> tags_;
	PurpleBlistNode *node = &m_buddy->node;
	while (!!(node = node->parent)) {
		if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
			PurpleGroup *group = PURPLE_GROUP(node);
			tags_.insert(group->name);
		}
	}
	if (m_tags != tags_) {
		m_tags = tags_;
		tagsChanged(m_tags);
	}
	QString name = purple_buddy_get_alias(m_buddy);
	if (name != m_name) {
		m_name = name;
		emit nameChanged(m_name);
	}
	Status status = quetzal_get_status(m_buddy->presence);
//	debug() << Q_FUNC_INFO << m_buddy->name << m_status << status;
	if (m_status.type() != status.type()
		|| m_status.subtype() != status.subtype()
		|| m_status.text() != status.text()) {
//		debug() << Q_FUNC_INFO << m_id << status;
		m_status = status;
		emit statusChanged(m_status);
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

QSet<QString> QuetzalContact::tags() const
{
	return m_tags;
}

Status QuetzalContact::status() const
{
	return m_status;
}

void QuetzalContact::sendMessage(const Message &message)
{
	PurpleConversation *conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM, m_buddy->name, m_buddy->account);
	if (!conv) {
		conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, m_buddy->account, m_buddy->name);
		conv->ui_data = this;
	}
	purple_conv_im_send(conv->u.im, message.text().toUtf8().constData());
}

void QuetzalContact::setName(const QString &name)
{
	purple_blist_alias_buddy(m_buddy, name.toUtf8().constData());
	serv_alias_buddy(m_buddy);
}

void QuetzalContact::setTags(const QSet<QString> &tags)
{
}

bool QuetzalContact::isInList() const
{
	return true;
}

void QuetzalContact::setInList(bool inList)
{
}

void quetzal_menu_add(QList<MenuController::Action> &actions, PurpleBlistNode *node,
					  GList *menu, const QList<QByteArray> &off, int type)
{
	int i = 0;
	for (GList *it = menu; it; it = it->next, i--) {
		PurpleMenuAction *action = (PurpleMenuAction *)it->data;
		actions << MenuController::Action((new QuetzalActionGenerator(action, node))
										  ->setType(type)->setPriority(i), off);
		QList<QByteArray> offs = off;
		offs.append(action->label);
		quetzal_menu_add(actions, node, action->children, offs, ActionGenerator::GeneralType);
		g_list_free(action->children);
		purple_menu_action_free(action);
	}
}

QList<MenuController::Action> QuetzalContact::dynamicActions() const
{
	QList<MenuController::Action> actions;
	if (!m_buddy->account->gc)
		return actions;
	PurpleBlistNode *node = const_cast<PurpleBlistNode *>(&m_buddy->node);
	GList *menu = NULL;
	PurplePluginProtocolInfo *prpl = PURPLE_PLUGIN_PROTOCOL_INFO(m_buddy->account->gc->prpl);
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
