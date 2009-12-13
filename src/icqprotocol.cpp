/****************************************************************************
 *  icqprotocol.cpp
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

#include "icq_global.h"
#include "util.h"
#include <qutim/icon.h>
#include "icqprotocol.h"
#include "icqaccount.h"
#include <QStringList>
#include <QPointer>
#include <QWizard>
#include <QWizardPage>
#include <QRegExp>
#include <QValidator>
#include <QDebug>

#include "ui_addaccountform.h"

namespace Icq {

IcqProtocol *IcqProtocol::self = 0;

class IcqAccWizardPage;

struct IcqAccWizardPrivate
{
	IcqAccWizardPage *page;
	IcqProtocol *protocol;
};

struct IcqProtocolPrivate
{
	inline IcqProtocolPrivate() : accounts_hash(new QHash<QString, QPointer<IcqAccount> >()) {}
	inline ~IcqProtocolPrivate() { delete accounts_hash; }
	union {
		QHash<QString, QPointer<IcqAccount> > *accounts_hash;
		QHash<QString, IcqAccount *> *accounts;
	};
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

qutim_sdk_0_3::Status icqStatusToQutim(quint16 status)
{
	if(status & IcqFlagInvisible)
		return Invisible;
	else /*if(status & IcqFlagEvil)
		return Evil;
	else if(status & IcqFlagDepress)
		return Depression;
	else if(status & IcqFlagHome)
		return AtHome;
	else if(status & IcqFlagWork)
		return AtWork;
	else if(status & IcqFlagLunch)
		return OutToLunch;
	else */if(status & IcqFlagDND)
		return DND;
	else if(status & IcqFlagOccupied)
		return Occupied;
	else if(status & IcqFlagNA)
		return NA;
	else if(status & IcqFlagAway)
		return Away;
	else if(status & IcqFlagFFC)
		return FreeChat;
	return Online;
}

IcqProtocol::IcqProtocol() : p(new IcqProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
}

IcqProtocol::~IcqProtocol()
{
	self = 0;
}

void initActions(IcqProtocol *proto)
{
	static bool inited = false;
	if(inited)
		return;
	QList<ActionGenerator *> actions;
	actions << (new ActionGenerator(Icon("user-online-icq"),
								   LocalizedString("Status", "Online"),
								   proto, SLOT(onStatusActionPressed())))->addProperty("status", Online)->setPriority(Online);
	actions << (new ActionGenerator(Icon("user-online-chat-icq"),
								   LocalizedString("Status", "Free for chat"),
								   proto, SLOT(onStatusActionPressed())))->addProperty("status", FreeChat)->setPriority(FreeChat);
	actions << (new ActionGenerator(Icon("user-away-icq"),
								   LocalizedString("Status", "Away"),
								   proto, SLOT(onStatusActionPressed())))->addProperty("status", Away)->setPriority(Away);
	actions << (new ActionGenerator(Icon("user-away-extended-icq"),
								   LocalizedString("Status", "NA"),
								   proto, SLOT(onStatusActionPressed())))->addProperty("status", NA)->setPriority(NA);
	actions << (new ActionGenerator(Icon("user-busy-icq"),
								   LocalizedString("Status", "DND"),
								   proto, SLOT(onStatusActionPressed())))->addProperty("status", DND)->setPriority(DND);
	actions << (new ActionGenerator(Icon("user-offline-icq"),
								   LocalizedString("Status", "Offline"),
								   proto, SLOT(onStatusActionPressed())))->addProperty("status", Offline)->setPriority(Offline);
	foreach (ActionGenerator *action, actions)
		MenuController::addAction(action, &IcqAccount::staticMetaObject);
	inited = true;
}

void IcqProtocol::loadAccounts()
{
	initActions(this);
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uin, accounts)
		p->accounts_hash->insert(uin, new IcqAccount(uin));
}

QList<Account *> IcqProtocol::accounts() const
{
    QList<Account *> accounts;
	QHash<QString, QPointer<IcqAccount> >::const_iterator it;
	for (it=p->accounts_hash->begin();it!=p->accounts_hash->end();it++)
		accounts.append(it.value());
    return accounts;
}
Account *IcqProtocol::account(const QString &id) const
{
    return p->accounts_hash->value(id);
}

void IcqProtocol::onStatusActionPressed()
{
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	MenuController *item = action->data().value<MenuController *>();
	if (IcqAccount *account = qobject_cast<IcqAccount *>(item)) {
		account->setStatus(static_cast<Status>(action->property("status").toInt()));
	}
}

} // namespace Icq
