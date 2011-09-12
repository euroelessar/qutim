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

#include "addcontactdialogwrapper.h"
#include "quickaddcontactdialog.h"
#include <qdeclarative.h>

namespace MeegoIntegration
{

Q_GLOBAL_STATIC(QList<AddContactDialogWrapper*>, m_managers)
QuickAddContactDialog* AddContactDialogWrapper::m_currentDialog;

AddContactDialogWrapper::AddContactDialogWrapper()
{
	m_managers()->append(this);
}

AddContactDialogWrapper::~AddContactDialogWrapper()
{
	m_managers()->removeOne(this);
}

void AddContactDialogWrapper::init()
{
	qmlRegisterType<AddContactDialogWrapper>("org.qutim", 0, 3, "AddContactDialogWrapper");
}

QString AddContactDialogWrapper::contactIdLabel()  const
{
	return m_dialogTitle;
}

QString AddContactDialogWrapper::contactIdText() const
{
	return m_passwordText;
}

QString AddContactDialogWrapper::contactNameText() const
{
	return m_rememberPassword;
}

void AddContactDialogWrapper::setContactIdLabel(const QString &idLabel)
{
	m_dialogTitle = title;
	emit contactIdLabelChanged();
}

void AddContactDialogWrapper::setContactIdText(const QString &idText)
{
	m_rememberPassword = rememberPassword;
	emit contactIdTextChanged();
}

void AddContactDialogWrapper::setcontactNameText(const QString &nameText)
{
	m_passwordText = password;
	emit contactNameTextChanged();
}

void AddContactDialogWrapper::accept()
{
	m_currentDialog->accept(passwordText(),rememberPassword());
}

void AddContactDialogWrapper::cancel()
{
	m_currentDialog->cancel();
}

void AddContactDialogWrapper::showDialog(QString title, QuickAddContactDialog * addContactDialog)
{
	m_currentDialog = addContactDialog;
	for (int i = 0; i < m_managers()->count();i++)
	{
		m_managers()->at(i)->setTitle(title);
		emit m_managers()->at(i)->shown();
	}
}
}

