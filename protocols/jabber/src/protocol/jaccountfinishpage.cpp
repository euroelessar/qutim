/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "jaccountfinishpage.h"
#include "ui_jaccountfinishpage.h"
#include "jaccountwizard.h"
#include <jreen/jid.h>

namespace Jabber
{

using namespace qutim_sdk_0_3;

JAccountFinishPage::JAccountFinishPage(JAccountWizard *accountWizard, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::JAccountFinishPage), m_accountWizard(accountWizard)
{
	ui->setupUi(this);
	setFinalPage(true);
}

JAccountFinishPage::~JAccountFinishPage()
{
	delete ui;
}

void JAccountFinishPage::initializePage()
{
	Jreen::JID jid = field(QLatin1String("jid")).toString();
	setSubTitle(tr("Congratulations! Account creation is finished, your Jabber ID is \"%1\"").arg(jid));
}

bool JAccountFinishPage::validatePage()
{
	m_accountWizard->createAccount();
	return true;
}

} // namespace Jaber
