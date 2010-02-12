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

QuetzalJoinChatDialog::QuetzalJoinChatDialog(PurpleConnection *gc, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QuetzalJoinChatDialog)
{
	m_gc = gc;
	ui->setupUi(this);
	init(NULL);
}

QuetzalJoinChatDialog::QuetzalJoinChatDialog(PurpleConnection *gc, const char *data, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QuetzalJoinChatDialog)
{
	m_gc = gc;
	ui->setupUi(this);
	init(data);
}

void QuetzalJoinChatDialog::init(const char *data)
{
	m_searchButton = ui->buttonBox->addButton(tr("Search"), QDialogButtonBox::ActionRole);
	m_joinButton = ui->buttonBox->addButton(tr("Join"), QDialogButtonBox::AcceptRole);
	connect(m_joinButton, SIGNAL(clicked()), this, SLOT(onJoinButtonClicked()));
	m_joinButton->setDefault(true);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);

	GHashTable *comps = NULL;
	GList *chats = quetzal_blist_get_chats(m_gc->account);
	if (chats) {
		comps = PURPLE_CHAT(chats->data)->components;
		ui->bookmarkName->setText(PURPLE_CHAT(chats->data)->alias);
	}
	for (GList *it = chats; it; it = it->next) {
		PurpleChat *chat = PURPLE_CHAT(it->data);
		ui->bookmarkBox->addItem(chat->alias, qVariantFromValue(chat));
	}
	g_list_free(chats);

	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_gc->prpl);
	if (info->chat_info != NULL) {
		GList *chat_info = info->chat_info(m_gc);
		PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(m_gc->prpl);
		if (comps == NULL && info->chat_info_defaults != NULL)
			comps = info->chat_info_defaults(m_gc, data);
		for (GList *it = chat_info; it; it = it->next) {
			proto_chat_entry *pce = reinterpret_cast<proto_chat_entry *>(it->data);
			const char *str = reinterpret_cast<const char *>(g_hash_table_lookup(comps, pce->identifier));
			QWidget *widget;
			if (pce->is_int) {
				QSpinBox *box = new QSpinBox(this);
				box->setMaximum(pce->max);
				box->setMinimum(pce->min);
				box->setValue(qBound(pce->min, str ? atoi(str) : 0, pce->max));
				widget = box;
			} else {
				QLineEdit *edit = new QLineEdit(str, this);
				if (pce->secret)
					edit->setEchoMode(QLineEdit::Password);
				widget = edit;
			}
			widget->setProperty("identifier", QByteArray(pce->identifier));
			char *label = purple_text_strip_mnemonic(pce->label);
			ui->fieldsLayout->addRow(label, widget);
			g_free(label);
			g_free(pce);
		}

		g_list_free(chat_info);
		if (ui->bookmarkBox->count() == 0)
			g_hash_table_destroy(comps);
	}
}

QuetzalJoinChatDialog::~QuetzalJoinChatDialog()
{
    delete ui;
}

void QuetzalJoinChatDialog::onJoinButtonClicked()
{
	GHashTable *table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	for (int i = 0, size = ui->fieldsLayout->rowCount(); i < size; i++) {
		QLayoutItem *item = ui->fieldsLayout->itemAt(i, QFormLayout::FieldRole);
		QWidget *widget = item->widget();
		QByteArray key = widget->property("identifier").toByteArray();
		QByteArray value;
		if (QSpinBox *box = qobject_cast<QSpinBox *>(widget)) {
			value = QByteArray::number(box->value());
		} else if (QLineEdit *edit = qobject_cast<QLineEdit *>(widget)){
			value = edit->text().toUtf8();
		} else {
			Q_ASSERT(!"Unknown widget, something strange has happend..");
		}
		g_hash_table_insert(table, g_strdup(key.constData()), g_strdup(value.constData()));
	}
	serv_join_chat(m_gc, table);
	if (ui->saveBookmark->isChecked()) {
		int index = ui->bookmarkBox->findText(ui->bookmarkName->text());
		PurpleChat *chat;
		if (index < 0) {
			chat = purple_chat_new(m_gc->account, ui->bookmarkName->text().toUtf8().constData(), table);
			purple_blist_add_chat(chat, NULL, NULL);
		} else {
			chat = ui->bookmarkBox->itemData(index).value<PurpleChat*>();
		}
	} else {
		g_hash_table_destroy(table);
	}
}

void QuetzalJoinChatDialog::on_bookmarkBox_currentIndexChanged(int index)
{
	PurpleChat *chat = ui->bookmarkBox->itemData(index).value<PurpleChat*>();
	for (int i = 0, size = ui->fieldsLayout->rowCount(); i < size; i++) {
		QLayoutItem *item = ui->fieldsLayout->itemAt(i, QFormLayout::FieldRole);
		QWidget *widget = item->widget();
		QByteArray key = widget->property("identifier").toByteArray();
		const char *str = reinterpret_cast<const char *>(g_hash_table_lookup(chat->components, key.constData()));
		if (QSpinBox *box = qobject_cast<QSpinBox *>(widget)) {
			box->setValue(str ? atoi(str) : 0);
		} else if (QLineEdit *edit = qobject_cast<QLineEdit *>(widget)){
			edit->setText(str);
		} else {
			Q_ASSERT(!"Unknown widget, something strange has happend..");
		}
	}
	ui->saveBookmark->setChecked(false);
	ui->bookmarkName->setText(ui->bookmarkBox->itemText(index));
}

void QuetzalJoinChatDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
		ui->retranslateUi(this);
		m_searchButton->setText(tr("Search"));
		m_joinButton->setText(tr("Join"));
        break;
    default:
        break;
    }
}
