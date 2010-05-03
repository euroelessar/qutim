#include "shortcutsettingswidget.h"
#include "ui_shortcutsettingswidget.h"
#include <libqutim/shortcut.h>
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
		QStandardItem *parent_item = m_model->invisibleRootItem();
		QStringList ids = Shortcut::ids();
		QHash <QString, QStandardItem*> groups;

		foreach (const QString &id,ids) {
			KeySequence sequence = Shortcut::getSequence(id);
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
			item->setEditable(true);
			group_item->appendRow(item);
		}
		ui->treeView->expandAll();
	}

	void ShortcutSettingsWidget::saveImpl()
	{

	}

	void ShortcutSettingsWidget::cancelImpl()
	{

	}
}
