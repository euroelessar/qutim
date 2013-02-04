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
#include "simplepassworddialog.h"
#include "ui_simplepasswordwidget.h"
#include <qutim/protocol.h>

namespace Core
{
SimplePasswordWidget::SimplePasswordWidget(Account *account, SimplePasswordDialog *parent) :
		ui(new Ui::SimplePasswordWidget)
{
	ui->setupUi(this);
	if (account) {
		ui->label->setText(tr("Enter password for account %1 (%2):")
		                   .arg(account->id(), account->protocol()->id()));
	}
	connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
	connect(this, SIGNAL(rejected()), this, SLOT(onReject()));
	m_account = account;
	m_parent = parent;
}

SimplePasswordWidget::~SimplePasswordWidget()
{
    delete ui;
}

void SimplePasswordWidget::setValidator(QValidator *validator)
{
	ui->passwordLineEdit->setValidator(validator);
}

void SimplePasswordWidget::showSaveButton(bool show)
{
	ui->rememberCheckBox->setVisible(show);
}

void SimplePasswordWidget::showLoginEdit(bool show)
{
	ui->loginLineEdit->setVisible(show);
	ui->label_3->setVisible(show);
}

void SimplePasswordWidget::setText(const QString &title, const QString &description)
{
	setWindowTitle(title);
	ui->label->setText(description);
}

void SimplePasswordWidget::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
		if (m_account) {
			ui->label->setText(tr("Enter password for account %1 (%2):")
			                   .arg(m_account->id(), m_account->protocol()->id()));
		}
        break;
    default:
        break;
    }
}

void SimplePasswordWidget::onAccept()
{
	m_parent->apply(ui->loginLineEdit->text(),
					ui->passwordLineEdit->text(),
					ui->rememberCheckBox->isChecked());
}

void SimplePasswordWidget::onReject()
{
	m_parent->reject();
}
}

