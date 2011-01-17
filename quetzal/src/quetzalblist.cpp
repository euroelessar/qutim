/****************************************************************************
 *  quetzalblist.cpp
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
#include "quetzalblist.h"
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

void quetzal_new_list(PurpleBuddyList *list)
{
	debug() << "new_list";
}

void quetzal_debug_pair(const char *key,
						const char *value,
						QDebug &debug)
{
	debug << key << value << "\n";
}

void quetzal_new_node(PurpleBlistNode *node)
{
//	debug() << "new_node" << node->type;
//	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
//		debug() << "BUDDY";
//		PurpleBuddy *buddy = PURPLE_BUDDY(node);
//		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data);
//		if (acc) {
//			acc->createNode(node);
//		}
//	} else if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
//		PurpleChat *chat = PURPLE_CHAT(node);
//		QDebug debug(QtDebugMsg);
//		debug << "CHAT";
//		g_hash_table_foreach(chat->components, (GHFunc)quetzal_debug_pair, &debug);
//	}
}

void quetzal_show(PurpleBuddyList *list)
{
	Q_UNUSED(list);
}

void quetzal_update(PurpleBuddyList *list, PurpleBlistNode *node)
{
	Q_UNUSED(list);
//	QObject *obj = reinterpret_cast<QObject *>(node->ui_data);
//	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
//		PurpleBuddy *buddy = PURPLE_BUDDY(node);
//		QuetzalAccount *account = reinterpret_cast<QuetzalAccount*>(buddy->account->ui_data);
//		if (!account)
//			return;
//		account->set
//		PurpleContact *contact = purple_buddy_get_contact(buddy);
//		PurpleGroup *group = purple_buddy_get_group(buddy);
//		qDebug() << "update" << obj << PURPLE_BUDDY(node)->name
//				<< contact->alias
//				<< group->name;
//	}
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		QObject *obj = reinterpret_cast<QObject *>(node->ui_data);
		if (QuetzalContact *contact = qobject_cast<QuetzalContact *>(obj)) {
			contact->update(PURPLE_BUDDY(node));
		}
	} else if (PURPLE_BLIST_NODE_IS_GROUP(node)) {
		// TODO: handle group name changes
//		PurpleGroup *group = PURPLE_GROUP(node);
//		group->node.child
	}
}

void quetzal_remove(PurpleBuddyList *list, PurpleBlistNode *node)
{
}

void quetzal_destroy(PurpleBuddyList *list)
{
}

void quetzal_set_visible(PurpleBuddyList *list, gboolean show)
{
}

void quetzal_request_add_buddy(PurpleAccount *account, const char *username, const char *group, const char *alias)
{
}

void quetzal_request_add_chat(PurpleAccount *account, PurpleGroup *group, const char *alias, const char *name)
{
}

void quetzal_request_add_group(void)
{
}

void quetzal_save_node(PurpleBlistNode *node)
{
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		PurpleBuddy *buddy = PURPLE_BUDDY(node);
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data);
		if (acc)
			acc->save(buddy);
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node)) {
		PurpleChat *chat = PURPLE_CHAT(node);
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(chat->account->ui_data);
		if (acc) {
			acc->save(chat);
		}
	}
}

void quetzal_remove_node(PurpleBlistNode *node)
{
	if (PURPLE_BLIST_NODE_IS_BUDDY(node)) {
		PurpleBuddy *buddy = PURPLE_BUDDY(node);
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(buddy->account->ui_data);
		if (acc)
			acc->remove(buddy);
	} else if(PURPLE_BLIST_NODE_IS_CHAT(node)) {
		PurpleChat *chat = PURPLE_CHAT(node);
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(chat->account->ui_data);
		if (acc) {
			acc->remove(chat);
		}
	}
}

void quetzal_save_account(PurpleAccount *account)
{
	if (account) {
		QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(account->ui_data);
		if (acc)
			acc->save();
	} else {
		for (GList *it = purple_accounts_get_all(); it != NULL; it = it->next) {
			account = reinterpret_cast<PurpleAccount *>(it->data);
			QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(account->ui_data);
			if (acc)
				acc->save();
		}
	}
}

PurpleBlistUiOps quetzal_blist_uiops = {
	quetzal_new_list,
	quetzal_new_node,
	quetzal_show,
	quetzal_update,
	quetzal_remove,
	quetzal_destroy,
	quetzal_set_visible,
	quetzal_request_add_buddy,
	quetzal_request_add_chat,
	quetzal_request_add_group,
	quetzal_save_node,
	quetzal_remove_node,
	quetzal_save_account,
	NULL
};

GList *quetzal_blist_get_chats(PurpleAccount *acc)
{
	PurpleBlistNode *group, *node;
	PurpleChat *chat;
	GList *list = NULL;
	for (group = purple_blist_get_root(); group; group = group->next) {
		for (node = group->child; node; node = node->next) {
			if (PURPLE_BLIST_NODE_IS_CHAT(node)) {
				chat = PURPLE_CHAT(node);
				if (chat->account != acc)
					continue;
				list = g_list_append(list, chat);
			}
		}
	}
	return list;
}
