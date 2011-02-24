#include "simpletagseditor.h"
#include <qutim/contact.h>
#include <qutim/icon.h>
#include "ui_simpletagseditor.h"

namespace Core {

SimpleTagsEditor::SimpleTagsEditor(Contact *contact) :
	QDialog(),
	ui(new Ui::SimpleTagsEditor),
	m_contact(contact)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	ui->addButton->setIcon(Icon("document-new"));
	setWindowIcon(Icon("feed-subscribe"));
	setWindowTitle(tr("Edit tags for %1").arg(contact->title()));
}

SimpleTagsEditor::~SimpleTagsEditor()
{
	delete ui;
}

void SimpleTagsEditor::accept()
{
	save();
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

void SimpleTagsEditor::load()
{
	QStringList tags = m_contact->tags();
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
	m_contact->setTags(tags.toList());
}

void SimpleTagsEditor::setTags(QStringList tags)
{
	m_additional_tags = tags;
}

void Core::SimpleTagsEditor::on_addButton_clicked()
{
	QListWidgetItem *item = new QListWidgetItem(ui->lineEdit->text(),ui->listWidget);
	item->setCheckState(Qt::Unchecked);
	ui->lineEdit->clear();
}

}
