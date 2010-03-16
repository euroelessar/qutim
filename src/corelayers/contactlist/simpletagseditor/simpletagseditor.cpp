#include "simpletagseditor.h"
#include <libqutim/contact.h>
#include <libqutim/icon.h>
#include "ui_simpletagseditor.h"

namespace Core {

	SimpleTagsEditor::SimpleTagsEditor(Contact *contact) :
		ui(new Ui::SimpleTagsEditor),
		m_contact(contact)
	{
		ui->setupUi(this);
		setAttribute(Qt::WA_DeleteOnClose);

		QActionGroup *group = new QActionGroup(this);
		QAction *act = new QAction(Icon("document-edit"),tr("Edit tags"),group);
		act->setCheckable(true);
		act->setData(0);

		act = new QAction(Icon("document-new"),tr("Add tag"),group);
		act->setCheckable(true);
		group->setExclusive(true);
		ui->actionBar->addActions(group->actions());
		act->setData(1);

		//FIXME
		ui->actionBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		ui->actionBar->setIconSize(QSize(32,32));

		connect(group,SIGNAL(triggered(QAction*)),SLOT(onActionTriggered(QAction*)));
		connect(ui->buttonBox,SIGNAL(accepted()),SLOT(onAddTagTriggered()));
		group->actions().first()->trigger();
	}

	SimpleTagsEditor::~SimpleTagsEditor()
	{
		save();
		delete ui;
	}

	void SimpleTagsEditor::changeEvent(QEvent *e)
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

	void SimpleTagsEditor::onActionTriggered(QAction *act)
	{		
		ui->stackedWidget->setCurrentIndex(act->data().toInt());
	}

	void SimpleTagsEditor::load()
	{
		QSet<QString> tags = m_contact->tags();
		ui->listWidget->clear();
		foreach (const QString &tag, tags) {
			QListWidgetItem *item = new QListWidgetItem(tag,ui->listWidget);
			item->setCheckState(Qt::Checked);
		}
		foreach (const QString &tag, m_additional_tags) {
			if (!tags.contains(tag)) {
				QListWidgetItem *item = new QListWidgetItem(tag,ui->listWidget);
				item->setCheckState(Qt::Unchecked);
			}
		}

	}

	void SimpleTagsEditor::save()
	{
		QSet<QString> tags;
		for (int index = 0;index!=ui->listWidget->count();index++) {
			if (ui->listWidget->item(index)->checkState() == Qt::Checked)
				tags << ui->listWidget->item(index)->text();
		}
		m_contact->setTags(tags);
	}

	void SimpleTagsEditor::onAddTagTriggered()
	{
		QListWidgetItem *item = new QListWidgetItem(ui->lineEdit->text(),ui->listWidget);
		item->setCheckState(Qt::Unchecked);
		//ui->actionBar->actions().first()->trigger();
		ui->lineEdit->clear();
	}

	void SimpleTagsEditor::setTags(QSet<QString>tags)
	{
		m_additional_tags = tags;
	}

}
