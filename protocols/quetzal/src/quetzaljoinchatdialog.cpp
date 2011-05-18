/****************************************************************************
 *  quetzaljoinchatdialog.cpp
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

#include "quetzaljoinchatdialog.h"
#include "ui_quetzaljoinchatdialog.h"
#include "quetzalblist.h"
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include "quetzalaccount.h"
#include <qutim/dataforms.h>

using namespace qutim_sdk_0_3;

QuetzalJoinChatManager::QuetzalJoinChatManager(QuetzalAccount *account) :
		GroupChatManager(account)
{
	m_gc = account->purple()->gc;
}

QuetzalJoinChatManager::~QuetzalJoinChatManager()
{
}

DataItem QuetzalJoinChatManager::fields() const
{
	GHashTable *comps = NULL;
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_gc->prpl);
	if (info->chat_info && info->chat_info_defaults)
		comps = info->chat_info_defaults(m_gc, NULL);
	DataItem item = fields(NULL, comps);
	g_hash_table_destroy(comps);
	return item;
}

DataItem QuetzalJoinChatManager::fields(const char *alias, GHashTable *comps) const
{
	DataItem item(alias ? QString::fromUtf8(alias) : QString());
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_gc->prpl);
	if (!info->chat_info)
		return item;
	
	GList *chat_info = info->chat_info(m_gc);
	for (GList *it = chat_info; it; it = it->next) {
		proto_chat_entry *pce = reinterpret_cast<proto_chat_entry *>(it->data);
		const char *str = reinterpret_cast<const char *>(g_hash_table_lookup(comps, pce->identifier));
		char *label = purple_text_strip_mnemonic(pce->label);
		int labelLength = qstrlen(label);
		// libpurples stores ':' at end of label, so we chop it by one char
		if (labelLength > 0)
			label[labelLength-1] = '\0';
		DataItem entry = DataItem(pce->identifier, label, QVariant());
		entry.setProperty("mandatory", pce->required);
		if (pce->is_int) {
			entry.setData(qBound(pce->min, str ? atoi(str) : 0, pce->max));
			entry.setProperty("minValue", pce->min);
			entry.setProperty("maxValue", pce->max);
		} else {
			entry.setData(QString::fromUtf8(str));
			entry.setProperty("password", bool(pce->secret));
		}
		g_free(label);
		g_free(pce);
		item.addSubitem(entry);
	}
	g_list_free(chat_info);
	return item;
}

template <typename GHashMethod>
void quetzal_chat_fill_components(GHashMethod method, GHashTable *comps, const DataItem &fields)
{
	foreach (const DataItem &item, fields.subitems()) {
		QByteArray key = item.name().toUtf8();
		QByteArray value = item.data().toString().toUtf8();
		method(comps, g_strdup(key.constData()), g_strdup(value.constData()));
	}
}

bool QuetzalJoinChatManager::join(const DataItem &fields)
{
	QuetzalChatGuard::Ptr guard = fields.property("quetzalPurpleChat", QuetzalChatGuard::Ptr());
	PurpleChat *chat = guard ? guard->chat : 0;
	if (chat) {
		serv_join_chat(m_gc, chat->components);
		return true;
	} else {
		GHashTable *comps = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		quetzal_chat_fill_components(g_hash_table_insert, comps, fields);
		serv_join_chat(m_gc, comps);
		PurpleChat *chat = purple_chat_new(m_gc->account, NULL, comps);
		PurpleGroup *group = purple_find_group("Recent");
		if (!group) {
			group = purple_group_new("Recent");
			purple_blist_add_group(group, NULL);
		}
		purple_blist_add_chat(chat, group, NULL);
//		g_hash_table_destroy(comps);
	}
	return true;
}

bool QuetzalJoinChatManager::storeBookmark(const DataItem &fields, const DataItem &oldFields)
{
	QuetzalChatGuard::Ptr guard = oldFields.property("quetzalPurpleChat", QuetzalChatGuard::Ptr());
	PurpleChat *chat = guard ? guard->chat : 0;
	if (chat) {
		GHashTable *comps = purple_chat_get_components(chat);
		quetzal_chat_fill_components(g_hash_table_replace, comps, fields);
	} else {
		GHashTable *comps = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		quetzal_chat_fill_components(g_hash_table_insert, comps, fields);
		chat = purple_chat_new(m_gc->account, NULL, comps);
		purple_blist_add_chat(chat, NULL, NULL);
	}
	return true;
}

bool QuetzalJoinChatManager::removeBookmark(const DataItem &fields)
{
	QuetzalChatGuard::Ptr guard = fields.property("quetzalPurpleChat", QuetzalChatGuard::Ptr());
	PurpleChat *chat = guard ? guard->chat : 0;
	if (!chat)
		return false;
	purple_blist_remove_chat(chat);
	return true;
}

QList<DataItem> QuetzalJoinChatManager::convertChats(bool recent) const
{
	QList<DataItem> items;
	GList *chats = quetzal_blist_get_chats(m_gc->account);
	for (GList *it = chats; it; it = it->next) {
		PurpleChat *chat = PURPLE_CHAT(it->data);
		QuetzalChatGuard::Ptr *guard = reinterpret_cast<QuetzalChatGuard::Ptr*>(chat->node.ui_data);
		if (recent == !g_strcmp0(purple_group_get_name(purple_chat_get_group(chat)), "Recent")) {
			DataItem item = fields(chat->alias, chat->components);
			item.setProperty("quetzalPurpleChat", qVariantFromValue(*guard));
			items << item;
		}
	}
	g_list_free(chats);
	return items;
}

QList<DataItem> QuetzalJoinChatManager::bookmarks() const
{
	return convertChats(false);
}

QList<DataItem> QuetzalJoinChatManager::recent() const
{
	return convertChats(true);
}

//void QuetzalJoinChatManager::init(const char *data)
//{
//	m_searchButton = ui->buttonBox->addButton(tr("Search"), QDialogButtonBox::ActionRole);
//	m_joinButton = ui->buttonBox->addButton(tr("Join"), QDialogButtonBox::AcceptRole);
//	connect(m_joinButton, SIGNAL(clicked()), this, SLOT(onJoinButtonClicked()));
//	m_joinButton->setDefault(true);
//	setAttribute(Qt::WA_DeleteOnClose, true);
//	setAttribute(Qt::WA_QuitOnClose, false);

//	GHashTable *comps = NULL;
//	GList *chats = quetzal_blist_get_chats(m_gc->account);
//	if (chats) {
//		comps = PURPLE_CHAT(chats->data)->components;
//		ui->bookmarkName->setText(PURPLE_CHAT(chats->data)->alias);
//	}
//	for (GList *it = chats; it; it = it->next) {
//		PurpleChat *chat = PURPLE_CHAT(it->data);
//		ui->bookmarkBox->addItem(chat->alias, qVariantFromValue(chat));
//	}
//	g_list_free(chats);

//	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_gc->prpl);
//	if (info->chat_info != NULL) {
//		GList *chat_info = info->chat_info(m_gc);
//		PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_gc->prpl);
//		if (comps == NULL && info->chat_info_defaults != NULL)
//			comps = info->chat_info_defaults(m_gc, data);
//		for (GList *it = chat_info; it; it = it->next) {
//			proto_chat_entry *pce = reinterpret_cast<proto_chat_entry *>(it->data);
//			const char *str = reinterpret_cast<const char *>(g_hash_table_lookup(comps, pce->identifier));
//			QWidget *widget;
//			if (pce->is_int) {
//				QSpinBox *box = new QSpinBox(this);
//				box->setMaximum(pce->max);
//				box->setMinimum(pce->min);
//				box->setValue(qBound(pce->min, str ? atoi(str) : 0, pce->max));
//				widget = box;
//			} else {
//				QLineEdit *edit = new QLineEdit(str, this);
//				if (pce->secret)
//					edit->setEchoMode(QLineEdit::Password);
//				widget = edit;
//			}
//			widget->setProperty("identifier", QByteArray(pce->identifier));
//			char *label = purple_text_strip_mnemonic(pce->label);
//			ui->fieldsLayout->addRow(label, widget);
//			g_free(label);
//			g_free(pce);
//		}

//		g_list_free(chat_info);
//		if (ui->bookmarkBox->count() == 0)
//			g_hash_table_destroy(comps);
//	}
//}

//void QuetzalJoinChatManager::onJoinButtonClicked()
//{
//	GHashTable *table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
//	for (int i = 0, size = ui->fieldsLayout->rowCount(); i < size; i++) {
//		QLayoutItem *item = ui->fieldsLayout->itemAt(i, QFormLayout::FieldRole);
//		QWidget *widget = item->widget();
//		QByteArray key = widget->property("identifier").toByteArray();
//		QByteArray value;
//		if (QSpinBox *box = qobject_cast<QSpinBox *>(widget)) {
//			value = QByteArray::number(box->value());
//		} else if (QLineEdit *edit = qobject_cast<QLineEdit *>(widget)){
//			value = edit->text().toUtf8();
//		} else {
//			Q_ASSERT(!"Unknown widget, something strange has happend..");
//		}
//		g_hash_table_insert(table, g_strdup(key.constData()), g_strdup(value.constData()));
//	}
//	serv_join_chat(m_gc, table);
//	if (ui->saveBookmark->isChecked()) {
//		int index = ui->bookmarkBox->findText(ui->bookmarkName->text());
//		PurpleChat *chat;
//		if (index < 0) {
//			chat = purple_chat_new(m_gc->account, ui->bookmarkName->text().toUtf8().constData(), table);
//			purple_blist_add_chat(chat, NULL, NULL);
//		} else {
//			chat = ui->bookmarkBox->itemData(index).value<PurpleChat*>();
//		}
//	} else {
//		g_hash_table_destroy(table);
//	}
//}

//void QuetzalJoinChatManager::on_bookmarkBox_currentIndexChanged(int index)
//{
//	PurpleChat *chat = ui->bookmarkBox->itemData(index).value<PurpleChat*>();
//	for (int i = 0, size = ui->fieldsLayout->rowCount(); i < size; i++) {
//		QLayoutItem *item = ui->fieldsLayout->itemAt(i, QFormLayout::FieldRole);
//		QWidget *widget = item->widget();
//		QByteArray key = widget->property("identifier").toByteArray();
//		const char *str = reinterpret_cast<const char *>(g_hash_table_lookup(chat->components, key.constData()));
//		if (QSpinBox *box = qobject_cast<QSpinBox *>(widget)) {
//			box->setValue(str ? atoi(str) : 0);
//		} else if (QLineEdit *edit = qobject_cast<QLineEdit *>(widget)){
//			edit->setText(str);
//		} else {
//			Q_ASSERT(!"Unknown widget, something strange has happend..");
//		}
//	}
//	ui->saveBookmark->setChecked(false);
//	ui->bookmarkName->setText(ui->bookmarkBox->itemText(index));
//}

//void QuetzalJoinChatManager::changeEvent(QEvent *e)
//{
//    QDialog::changeEvent(e);
//    switch (e->type()) {
//    case QEvent::LanguageChange:
//		ui->retranslateUi(this);
//		m_searchButton->setText(tr("Search"));
//		m_joinButton->setText(tr("Join"));
//        break;
//    default:
//        break;
//    }
//}
