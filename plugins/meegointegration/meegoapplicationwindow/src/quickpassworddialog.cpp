/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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
#include <qdeclarative.h>
#include <qutim/account.h>
#include <qutim/protocol.h>

namespace MeegoIntegration
{
QuickPasswordDialog::QuickPasswordDialog() {
	m_dialogTitle = tr("Enter password for account");
	m_templateTitle = tr("Enter password for account %1 (%2)");
	m_rememberPassword = false;
	qDebug()<<"dkbnsdkjbnsdkbnsdkvsdkvsdvlsdmvsmdvsdmlvsdmlvsdmvsmd";

}

void QuickPasswordDialog::init(){
	qmlRegisterType<QuickPasswordDialog>("org.qutim", 0, 3, "QuickPasswordDialog");
}

QuickPasswordDialog::~QuickPasswordDialog() {
}

QString QuickPasswordDialog::title() {
	return m_dialogTitle;
}

QString QuickPasswordDialog::passwordText() {
	return m_passwordText;
}

bool QuickPasswordDialog::rememberPassword()
{
	return m_rememberPassword;
}

void QuickPasswordDialog::setRememberPassword(bool rememberPassword)
{
	m_rememberPassword = rememberPassword;
	emit rememberPasswordChanged(m_rememberPassword);
}

void QuickPasswordDialog::setPasswordText(QString passwordText)
{
	m_passwordText = passwordText;
	emit passwordTextChanged(passwordText);
}

void QuickPasswordDialog::accept() {
	this->apply("text",m_rememberPassword);
}

void QuickPasswordDialog::cancel() {
	this->reject();

}

void QuickPasswordDialog::setAccount(Account *account)
{
	m_dialogTitle = m_templateTitle.arg(account->id(), account->protocol()->id());
	emit titleChanged(m_dialogTitle);
	emit shown();
}

void QuickPasswordDialog::setValidator(QValidator *validator)
{
	Q_UNUSED(validator)
}
}
