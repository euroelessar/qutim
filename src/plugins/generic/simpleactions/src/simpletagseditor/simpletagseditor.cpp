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
#include "simpletagseditor.h"
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/account.h>
#include <qutim/accountmanager.h>
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
	QDialog::accept();
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
	QStringList tags;
	// Try to request tag list from the contact model
	if (QObject *model = ServiceManager::getByName("ContactModel"))
		tags = model->property("tags").toStringList();

	if (tags.isEmpty()) {
		// There is no ContactModel service, or it does not have
		// tags() method, so we pass all contacts to gather tag list.
		// It is slow, but it's better than nothing.
		QSet<QString> tagsSet;
		foreach (Account *account, AccountManager::instance()->accounts()) {
			foreach (Contact *contact, account->findChildren<Contact*>()) {
				tagsSet += contact->tags().toSet();
			}
		}
		tags = tagsSet.toList();
	}

	QStringList contactTags = m_contact->tags();
	ui->listWidget->clear();
	foreach (const QString &tag, tags) {
		QListWidgetItem *item = new QListWidgetItem(tag,ui->listWidget);
		item->setCheckState(contactTags.contains(tag) ? Qt::Checked : Qt::Unchecked);
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

void Core::SimpleTagsEditor::on_addButton_clicked()
{
	QListWidgetItem *item = new QListWidgetItem(ui->lineEdit->text(),ui->listWidget);
	item->setCheckState(Qt::Unchecked);
	ui->lineEdit->clear();
}

}

