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

#include "quickpassworddialogmanager.h"
#include "quickpassworddialog.h"
#include <qdeclarative.h>

namespace MeegoIntegration
{

QuickPasswordDialogManager::QuickPasswordDialogManager() {
	m_dialogTitle = tr("Enter password for account");

	m_rememberPassword = false;
	m_managers.append(this);
}
QuickPasswordDialogManager::~QuickPasswordDialogManager()
{
	m_managers.removeOne(this);
}

void QuickPasswordDialogManager::init(){
	qmlRegisterType<QuickPasswordDialogManager>("org.qutim", 0, 3, "QuickPasswordDialog");
}

QString QuickPasswordDialogManager::title() {
	return m_dialogTitle;
}

QString QuickPasswordDialogManager::passwordText() {
	return m_passwordText;
}

bool QuickPasswordDialogManager::rememberPassword()
{
	return m_rememberPassword;
}

void QuickPasswordDialogManager::setTitle(QString title)
{
	m_dialogTitle = title;
	emit titleChanged(m_dialogTitle);
}

void QuickPasswordDialogManager::setRememberPassword(bool rememberPassword)
{
	m_rememberPassword = rememberPassword;
	emit rememberPasswordChanged(m_rememberPassword);
}

void QuickPasswordDialogManager::setPasswordText(QString passwordText)
{
	m_passwordText = passwordText;
	emit passwordTextChanged(passwordText);
}

void QuickPasswordDialogManager::accept() {
	m_currentDialog->accept(m_passwordText,m_rememberPassword);
}

void QuickPasswordDialogManager::cancel() {
	m_currentDialog->cancel();

}

void QuickPasswordDialogManager::showDialog(QString title,QuickPasswordDialog * passDialog)
{
	m_currentDialog = passDialog;
	for (int i = 0; i = m_managers.count();i++)
	{
		m_managers[i]->setTitle(title);
		emit m_managers[i]->shown();
	}

}

}

