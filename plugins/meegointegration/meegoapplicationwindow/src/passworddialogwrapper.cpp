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

#include "passworddialogwrapper.h"
#include "quickpassworddialog.h"
#include <qdeclarative.h>

namespace MeegoIntegration
{

Q_GLOBAL_STATIC(QList<PasswordDialogWrapper*>, m_managers)
QuickPasswordDialog* PasswordDialogWrapper::m_currentDialog;

PasswordDialogWrapper::PasswordDialogWrapper()
{
	m_dialogTitle = tr("Enter password for account");
	m_managers()->append(this);
}

PasswordDialogWrapper::~PasswordDialogWrapper()
{
	m_managers()->removeOne(this);
}

void PasswordDialogWrapper::init()
{
	qmlRegisterType<PasswordDialogWrapper>("org.qutim", 0, 3, "PasswordDialogWrapper");
}

QString PasswordDialogWrapper::title()  const
{
	return m_dialogTitle;
}

QString PasswordDialogWrapper::passwordText() const
{
	return m_passwordText;
}

bool PasswordDialogWrapper::rememberPassword()
{
	return m_rememberPassword;
}

void PasswordDialogWrapper::setTitle(const QString &title)
{
	m_dialogTitle = title;
	emit titleChanged();
}

void PasswordDialogWrapper::setRememberPassword(bool rememberPassword)
{
	m_rememberPassword = rememberPassword;
	emit rememberPasswordChanged();
}

void PasswordDialogWrapper::setPasswordText(const QString &password)
{
	m_passwordText = password;
	emit passwordTextChanged();
}

void PasswordDialogWrapper::accept()
{
	m_currentDialog->accept(passwordText(),rememberPassword());
}

void PasswordDialogWrapper::cancel()
{
	m_currentDialog->cancel();
}

void PasswordDialogWrapper::showDialog(QString title, QuickPasswordDialog * passDialog)
{
	m_currentDialog = passDialog;
	for (int i = 0; i < m_managers()->count();i++)
	{
		m_managers()->at(i)->setTitle(title);
		emit m_managers()->at(i)->shown();
	}
}
}

