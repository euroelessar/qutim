/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "mergedialog.h"
#include "ui_mergedialog.h"
#include "model.h"
#include <itemdelegate.h>
#include "manager.h"

namespace Core {
namespace MetaContacts {

using namespace qutim_sdk_0_3;

MergeDialog::MergeDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::MergeDialog)
{
    ui->setupUi(this);
	ui->treeView->setModel(m_model = new Model(this));
	ui->treeView->setItemDelegate(new ItemDelegate(this));
	ui->treeView->expandAll();
	
	connect(ui->searchField, SIGNAL(textChanged(QString)),
			m_model, SLOT(searchContacts(QString)));
	connect(ui->treeView, SIGNAL(clicked(QModelIndex)),
			m_model, SLOT(activated(QModelIndex)));
	connect(m_model, SIGNAL(addContactTriggered(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
	connect(m_model, SIGNAL(removeContactTriggered(qutim_sdk_0_3::Contact*)),
			this, SLOT(removeContact(qutim_sdk_0_3::Contact*)));
}

MergeDialog::~MergeDialog()
{
    delete ui;
}

void MergeDialog::setMetaContact(MetaContactImpl* contact)
{
	if(m_model->metaContact())
		m_model->metaContact()->disconnect(this);
	connect(contact, SIGNAL(nameChanged(QString,QString)),
			this, SLOT(nameChanged(QString)));

	m_model->setMetaContact(contact);
	nameChanged(contact->name());
}

void MergeDialog::addContact(qutim_sdk_0_3::Contact *contact)
{
	MetaContactImpl *metaContact = m_model->metaContact();
	if (!metaContact) {
		metaContact = static_cast<MetaContactImpl*>(MetaContactManager::instance()->createContact());
		metaContact->addContact(contact);
		setMetaContact(metaContact);
	} else
		metaContact->addContact(contact);

}

void MergeDialog::removeContact(qutim_sdk_0_3::Contact *contact)
{
	MetaContactImpl *metaContact = m_model->metaContact();
	Q_ASSERT(metaContact);
	metaContact->removeContact(contact);
}

void MergeDialog::setName(const QString& name)
{
	if(m_model->metaContact())
		m_model->metaContact()->setName(name);
}

void MergeDialog::closeEvent(QCloseEvent *ev)
{
	setName(ui->nameEdit->text());
    QWidget::closeEvent(ev);
}

void MergeDialog::nameChanged(const QString& name)
{
	ui->nameEdit->setText(name);
	setWindowFilePath(name);
}


} // namespace MetaContacts
} // namespace Core

