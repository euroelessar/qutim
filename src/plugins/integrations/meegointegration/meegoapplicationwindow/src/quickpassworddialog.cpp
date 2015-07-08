/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "quickpassworddialog.h"
#include "passworddialogwrapper.h"
#include <qdeclarative.h>
#include <qutim/account.h>
#include <qutim/protocol.h>

namespace MeegoIntegration
{
QuickPasswordDialog::QuickPasswordDialog() {
	m_templateTitle = tr("Enter password for account \n %1 (%2)");
}

void QuickPasswordDialog::accept(const QString &password, bool remember)
{
	this->apply(password,remember);
}

void QuickPasswordDialog::cancel()
{
	this->reject();
}

void QuickPasswordDialog::setAccount(Account *account)
{
	PasswordDialogWrapper::showDialog(m_templateTitle.arg(account->id(), account->protocol()->id()),this);
}

void QuickPasswordDialog::setValidator(QValidator *validator)
{
	Q_UNUSED(validator)
}
}

