/****************************************************************************
 *  accountcreator.cpp
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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
#include "ircprotocol.h"
#include "ircaccount.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcAccWizardPage::IrcAccWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent) :
	IrcAccountMainSettings(parent), m_accountWizard(accountWizard)
{

}

IrcAccWizardPage::~IrcAccWizardPage()
{
}

bool IrcAccWizardPage::isComplete() const
{
	QString network = networkName();
	if (network.isEmpty() || IrcProtocol::instance()->account(network) || servers().isEmpty())
		return false;
	return IrcAccountMainSettings::isComplete();
}

bool IrcAccWizardPage::validatePage()
{
	return isComplete();
}

IrcNickWizardPage::IrcNickWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent) :
	IrcAccountNickSettings(parent), m_accountWizard(accountWizard)
{
}

IrcNickWizardPage::~IrcNickWizardPage()
{
}

bool IrcNickWizardPage::validatePage()
{
	if (!nicks().isEmpty()) {
		m_accountWizard->finished();
		return true;
	}
	return false;
}

IrcAccountCreationWizard::IrcAccountCreationWizard() :
	AccountCreationWizard(IrcProtocol::instance())
{
}

IrcAccountCreationWizard::~IrcAccountCreationWizard()
{
}

QList<QWizardPage *> IrcAccountCreationWizard::createPages(QWidget *parent)
{
	m_mainPage = new IrcAccWizardPage(this, parent);
	m_nicksPage = new IrcNickWizardPage(this, parent);
	QList<QWizardPage *> pages;
	pages << m_mainPage;
	pages << m_nicksPage;
	return pages;
}

void IrcAccountCreationWizard::finished()
{
	IrcAccount *account = IrcProtocol::instance()->getAccount(m_mainPage->networkName(), true);
	// Account config.
	Config cfg = account->config();
	m_mainPage->saveToConfig(cfg);
	m_nicksPage->saveToConfig(cfg);
	// Protocol config.
	cfg = IrcProtocol::instance()->config("general");
	QStringList accounts = cfg.value("accounts", QStringList());
	accounts << account->id();
	cfg.setValue("accounts", accounts);
	account->updateSettings();
}

} } // namespace qutim_sdk_0_3::irc
