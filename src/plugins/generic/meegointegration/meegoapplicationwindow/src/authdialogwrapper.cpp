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

#include "authdialogwrapper.h"
#include "quickauthdialog.h"
#include <qdeclarative.h>

namespace MeegoIntegration
{

Q_GLOBAL_STATIC(QList<AuthDialogWrapper*>, m_managers)
QuickAuthDialog* AuthDialogWrapper::m_currentDialog;

AuthDialogWrapper::AuthDialogWrapper()
{
	m_managers()->append(this);
}

AuthDialogWrapper::~AuthDialogWrapper()
{
	m_managers()->removeOne(this);
}

void AuthDialogWrapper::init()
{
	qmlRegisterType<AuthDialogWrapper>("org.qutim", 0, 3, "AuthDialogWrapper");
}

QString AuthDialogWrapper::title()  const
{
	return m_dialogTitle;
}

QString AuthDialogWrapper::messageText() const
{
	return m_messageText;
}

bool AuthDialogWrapper::isIncoming()
{
	return m_isIncoming;
}

Contact* AuthDialogWrapper::contact()
{
	return m_contact;
}

void AuthDialogWrapper::setTitle(const QString &title)
{
	m_dialogTitle = title;
	emit titleChanged();
}

void AuthDialogWrapper::setIsIncoming(bool isIncoming)
{
	m_isIncoming = isIncoming;
	emit isIncomingChanged();
}

void AuthDialogWrapper::setMessageText(const QString &message)
{
	m_messageText = message;
	emit messageTextChanged();
}

void AuthDialogWrapper::setContact(Contact *contact)
{
	m_contact = contact;
	emit contactChanged();
}

void AuthDialogWrapper::accept()
{
	m_currentDialog->accept(messageText(),isIncoming(),contact());
}

void AuthDialogWrapper::cancel()
{
	m_currentDialog->cancel(contact(),isIncoming());
}

void AuthDialogWrapper::showDialog(qutim_sdk_0_3::Contact* contact, const QString message, bool isIncoming, QuickAuthDialog * authDialog)
{
	m_currentDialog = authDialog;
	for (int i = 0; i < m_managers()->count();i++)
	{
		AuthDialogWrapper *current = m_managers()->at(i);
		if (isIncoming) {
			current->setTitle(QT_TRANSLATE_NOOP("ContactInfo", "Received authorization request from %1:")
					.toString().arg(contact->title()));
		} else {
			current->setTitle(QT_TRANSLATE_NOOP("ContactInfo", "Send authorization request to %1:")
					.toString().arg(contact->title()));
		}
		current->setMessageText(message);
		current->setIsIncoming(isIncoming);
		current->setContact(contact);
		emit m_managers()->at(i)->shown();
	}
}
}

