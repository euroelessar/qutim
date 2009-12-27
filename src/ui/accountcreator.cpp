/****************************************************************************
 *  accountcreator.cpp
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "accountcreator.h"
#include "icqprotocol_p.h"
#include <QWizard>
#include <QWizardPage>
#include <QRegExp>
#include <QValidator>

#include "ui_addaccountform.h"

namespace Icq {

class IcqAccWizardPage;

struct IcqAccWizardPrivate
{
	IcqAccWizardPage *page;
	IcqProtocol *protocol;
};

class IcqAccWizardPage: public QWizardPage
{
public:
	IcqAccWizardPage(IcqAccountCreationWizard *account_wizard);
	bool validatePage();
	QString uin() { return ui.uinEdit->text(); }
	QString password() { return ui.passwordEdit->text(); }
	bool isSavePassword() { return ui.passwordBox->isChecked(); }
private:
	IcqAccountCreationWizard *m_account_wizard;
	Ui::AddAccountFormClass ui;
};

IcqAccWizardPage::IcqAccWizardPage(IcqAccountCreationWizard *account_wizard):
	m_account_wizard(account_wizard)
{
	ui.setupUi(this);

	QRegExp rx("[1-9][0-9]{1,9}");
	QValidator *validator = new QRegExpValidator(rx, this);
	ui.uinEdit->setValidator(validator);
}

bool IcqAccWizardPage::validatePage()
{
	if(uin().isEmpty() || (isSavePassword() && password().isEmpty()))
		return false;
	m_account_wizard->finished();
	return true;
}

IcqAccountCreationWizard::IcqAccountCreationWizard()
		: AccountCreationWizard(IcqProtocol::instance()), p(new IcqAccWizardPrivate)
{
	p->protocol = IcqProtocol::instance();
}

IcqAccountCreationWizard::~IcqAccountCreationWizard()
{

}

QList<QWizardPage *> IcqAccountCreationWizard::createPages(QWidget *parent)
{
	p->page = new IcqAccWizardPage(this);
	QList<QWizardPage *> pages;
	pages << p->page;
	return pages;
}

void IcqAccountCreationWizard::finished()
{
	IcqAccount *account = new IcqAccount(p->page->uin());
	if(p->page->isSavePassword())
	{
		account->config().group("general").setValue("passwd", p->page->password(), Config::Crypted);
		account->config().sync();
	}
	ConfigGroup cfg = p->protocol->config().group("general");
	QStringList accounts = cfg.value("accounts", QStringList());
	accounts << account->id();
	cfg.setValue("accounts", accounts);
	cfg.sync();
	p->protocol->p->accounts_hash->insert(account->id(), account);
	delete p->page;
	emit p->protocol->accountCreated(account);
}

} // namespace Icq
