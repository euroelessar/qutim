/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include "ircaccountmainsettings.h"
#include "ircaccountnicksettings.h"

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccountCreationWizard;

class IrcAccWizardPage: public IrcAccountMainSettings
{
	Q_OBJECT
public:
	IrcAccWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent = 0);
	~IrcAccWizardPage();
	bool isComplete() const;
	bool validatePage();
private:
	IrcAccountCreationWizard *m_accountWizard;
};

class IrcNickWizardPage: public IrcAccountNickSettings
{
	Q_OBJECT
public:
	IrcNickWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent = 0);
	~IrcNickWizardPage();
	bool validatePage();
private:
	IrcAccountCreationWizard *m_accountWizard;
};

class IrcAccountCreationWizard: public AccountCreationWizard
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::irc::IrcProtocol")
public:
	IrcAccountCreationWizard();
	~IrcAccountCreationWizard();
	QList<QWizardPage *> createPages(QWidget *parent);
	void finished();
private:
	IrcAccWizardPage *m_mainPage;
	IrcNickWizardPage *m_nicksPage;
};

} } // namespace qutim_sdk_0_3::irc

#endif // ACCOUNTCREATOR_H

