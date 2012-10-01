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
#include "shortcutsettingswidget.h"
#include "ui_shortcutsettingswidget.h"
#include <qutim/shortcut.h>
#include <QTreeWidgetItem>
#include <QStandardItemModel>
#include "shortcutitemdelegate.h"

namespace Core
{

	ShortcutSettingsWidget::ShortcutSettingsWidget() :
			ui(new Ui::ShortcutSettingsWidget)
	{
		ui->setupUi(this);
		m_model = new QStandardItemModel(ui->treeView);
		ui->treeView->setModel(m_model);
		ui->treeView->setItemDelegate(new ShortcutItemDelegate(ui->treeView));
	
		connect(m_model,SIGNAL(itemChanged(QStandardItem*)),SLOT(onItemChanged(QStandardItem*)));
	}

	ShortcutSettingsWidget::~ShortcutSettingsWidget()
	{
		delete ui;
	}

	void ShortcutSettingsWidget::changeEvent(QEvent *e)
	{
		QWidget::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}

	void ShortcutSettingsWidget::loadImpl()
	{
		m_model->clear();
		QHash <QString, QStandardItem*> groups;
		fillModel(groups,false);
		fillModel(groups,true);
		ui->treeView->expandAll();
	}

	void ShortcutSettingsWidget::fillModel(QHash<QString, QStandardItem *> &groups,
										   bool global)
	{
		QStringList ids = global ? GlobalShortcut::ids() : Shortcut::ids();
		QStandardItem *parent_item = m_model->invisibleRootItem();
		foreach (const QString &id,ids) {
			KeySequence sequence = global ? GlobalShortcut::getSequence(id) : Shortcut::getSequence(id);
			QStandardItem *group_item = 0;
			if (!groups.contains(sequence.group)) {
				group_item = new QStandardItem();
				group_item->setText(sequence.group);
				group_item->setData(true,ShortcutItemDelegate::GroupRole);
				group_item->setEditable(false);
				groups.insert(sequence.group,group_item);
				parent_item->appendRow(group_item);
			} else
				group_item = groups.value(sequence.group);

			QStandardItem *item = new QStandardItem();
			item->setText(sequence.name);
			item->setData(sequence.key,ShortcutItemDelegate::SequenceRole);
			item->setData(sequence.id,ShortcutItemDelegate::IdRole);
			item->setData(global,ShortcutItemDelegate::GlobalRole);
			item->setEditable(true);
			group_item->appendRow(item);
		}

	}

	void ShortcutSettingsWidget::saveImpl()
	{
		foreach (QStandardItem *item,m_changed_items) {
			QString id = item->data(ShortcutItemDelegate::IdRole).toString();
			bool global = item->data(ShortcutItemDelegate::GlobalRole).toBool();
			QKeySequence sequence = item->data(ShortcutItemDelegate::SequenceRole).value<QKeySequence>();
			if (global)
				GlobalShortcut::setSequence(id,sequence);
			else
				Shortcut::setSequence(id,sequence);
		}
	}

	void ShortcutSettingsWidget::cancelImpl()
	{

	}
	
	void ShortcutSettingsWidget::onItemChanged ( QStandardItem* item )
	{
		setModified(true);
		if (!m_changed_items.contains(item))
			m_changed_items.append(item);
	}

}

