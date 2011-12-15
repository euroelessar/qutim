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
#include "accountwizardmain.h"
#include "ui_accountwizardmain.h"
#include "../../base/mrimprotocol.h"

AccountWizardMain::AccountWizardMain(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::AccountWizardMain)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    registerField("email*", ui->emailEdit);
    registerField("pass*", ui->passEdit);
}

AccountWizardMain::~AccountWizardMain()
{
    delete ui;
}

void AccountWizardMain::changeEvent(QEvent *e)
{
    QWizardPage::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool AccountWizardMain::validatePage()
{
    QWizardPage::validatePage();

    bool ret = false;

    if (isFinalPage())
    {
	QString email = field("email").value<QString>().toLower().trimmed() + ui->domainComboBox->currentText();
        MrimProtocol::AccountCreationError err = MrimProtocol::instance()->createAccount(email,field("pass").value<QString>());

        if (err == MrimProtocol::None)
        {
            ret = true;
        }
        else
        {
        //TODO: show message box with error
        }
    }
    return ret;
}

