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
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/messagesession.h>
#include <qutim/servicemanager.h>
#include <QStringBuilder>


namespace MeegoIntegration
{
enum {
	ChannelRole = Qt::UserRole,
	UnreadCountRole
};

Q_GLOBAL_STATIC(QList<AddContactDialogWrapper*>, m_managers)
QuickAddContactDialog* AddContactDialogWrapper::m_currentDialog;

AddContactDialogWrapper::AddContactDialogWrapper()
{
	m_managers()->append(this);

	foreach (Protocol *protocol, Protocol::all())
		if (protocol->data(Protocol::ProtocolContainsContacts).toBool() && !protocol->accounts().isEmpty())
			foreach (Account *acc, protocol->accounts()) {
				m_accounts.insert(acc->id(),acc);
			}
	if (m_accounts.count() == 1)
		setAccount(m_accounts.values().at(0));
	else
		m_showAccountsList = true;

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
	return m_idLabel;
}

bool AddContactDialogWrapper::showAccountsList()  const
{
	return m_showAccountsList;
}

void AddContactDialogWrapper::setAccount(QString accountId)
{
	foreach (Account *acc,m_accounts)
	{
		if (acc->id() == accountId)
		{
			currentAccount = acc;
			setContactIdLabel(acc->id());
			m_showAccountsList = false;
			emit showAccountsListChanged();
			break;
		}
	}
}

void AddContactDialogWrapper::setContactIdLabel(const QString &idLabel)
{
	m_idLabel = idLabel;
	emit contactIdLabelChanged();
}


void AddContactDialogWrapper::addContact(QString id, QString name)
{
	Contact *contact = qobject_cast<Contact *>(currentAccount->getUnit(id, true));
	if (contact) {
		contact->setInList(true);
		contact->setName(name);
	}
}

void AddContactDialogWrapper::showDialog(QuickAddContactDialog * addContactDialog)
{
	m_currentDialog = addContactDialog;
	for (int i = 0; i < m_managers()->count();i++)
	{
		emit m_managers()->at(i)->shown();
	}
}

int AddContactDialogWrapper::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_accounts.size();
}

QVariant AddContactDialogWrapper::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() > m_accounts.size())
		return QVariant();
	Account *account = m_accounts[index.row()];
	switch (role) {
	case Qt::DisplayRole:
		return account->unit()->title();
	case Qt::DecorationRole:
		return QString();
	case ChannelRole:
		return qVariantFromValue<QObject*>(account);
	case UnreadCountRole:
		return account->unread().count();
	default:
		return QVariant();
	}
}

}

