/****************************************************************************
 *  passworddialog.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "passworddialog.h"
#include <QLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include "icqaccount.h"
#include "ui_passwordform.h"

namespace qutim_sdk_0_3 {

namespace oscar {

PasswordDialog::PasswordDialog(IcqAccount *account, QWidget *parent) :
	QDialog(parent), m_account(account)
{
	ui = new Ui::PasswordForm;
	QWidget *form = new QWidget(this);
	ui->setupUi(form);
	QVBoxLayout *layout = new QVBoxLayout(this);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
							   QDialogButtonBox::Cancel,
							   Qt::Horizontal, this);
	m_okButton = buttonBox->button(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), SLOT(accepted()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
	connect(ui->passwordEdit, SIGNAL(textChanged(QString)), SLOT(validate()));
	connect(ui->passwordBox, SIGNAL(clicked()), SLOT(validate()));
	layout->addWidget(form);
	layout->addWidget(buttonBox);
	m_okButton->setEnabled(false);
	ui->passwordEdit->setFocus();
	setWindowTitle(account->id());
	resize(form->size());
}

PasswordDialog::~PasswordDialog()
{
	delete ui;
}

QString PasswordDialog::password() const
{
	return ui->passwordEdit->text();
}

bool PasswordDialog::isSavePassword() const
{
	return ui->passwordBox->isChecked();
}

void PasswordDialog::accepted()
{
	if (isSavePassword()) {
		m_account->config().group("general").setValue("passwd", password(), Config::Crypted);
		m_account->config().sync();
	}
}

void PasswordDialog::validate()
{
	bool disabled = password().isEmpty() && isSavePassword();
	m_okButton->setEnabled(!disabled);
}

} } // namespace qutim_sdk_0_3::oscar
