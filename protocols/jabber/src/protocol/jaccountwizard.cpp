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
#include "jaccountwizard.h"
#include "jaccountwizardpage.h"
#include "jaccountregistrationpage.h"
#include "jaccountfinishpage.h"
#include "jprotocol.h"
#include "account/jaccount.h"

namespace Jabber
{

	JAccountWizard::JAccountWizard() : AccountCreationWizard(JProtocol::instance())
	{
		protocol = JProtocol::instance();
		type = AccountTypeJabber;
	}

	JAccountWizard::~JAccountWizard()
	{
	}

	QList<QWizardPage *> JAccountWizard::createPages(QWidget *parent)
	{
		page = new JAccountWizardPage(this, type, parent);
		QList<QWizardPage *> pages;
		pages.append(page);
		pages.append(new JAccountRegistrationPage(parent));
		pages.append(new JAccountFinishPage(this, parent));
		return pages;
	}

	void JAccountWizard::createAccount()
	{
		JAccount *account = new JAccount(page->jid());
		if(page->isSavePasswd())
			account->setPasswd(page->passwd());

		Config cfg = protocol->config().group("general");
		QStringList accounts = cfg.value("accounts", QStringList());
		accounts << account->id();
		cfg.setValue("accounts", accounts);
		cfg.sync();
		protocol->addAccount(account, true);
		delete page;
	}

	LJAccountWizard::LJAccountWizard() : JAccountWizard()
	{
		type = AccountTypeLivejournal;
		ExtensionInfo info("LiveJournal", "Add LiveJournal account");
		setInfo(info);
	}

	LJAccountWizard::~LJAccountWizard()
	{
	}

	GTAccountWizard::GTAccountWizard() : JAccountWizard()
	{
		type = AccountTypeGoogletalk;
		ExtensionInfo info("GoogleTalk", "Add GoogleTalk account");
		setInfo(info);
	}

	GTAccountWizard::~GTAccountWizard()
	{
	}

	YAccountWizard::YAccountWizard() : JAccountWizard()
	{
		type = AccountTypeYandex;
		ExtensionInfo info("Yandex.Online", "Add Yandex.Online account");
		setInfo(info);
	}

	YAccountWizard::~YAccountWizard()
	{
	}

	QIPAccountWizard::QIPAccountWizard() : JAccountWizard()
	{
		type = AccountTypeQip;
		ExtensionInfo info("QIP", "Add QIP account");
		setInfo(info);
	}

	QIPAccountWizard::~QIPAccountWizard()
	{
	}
}

