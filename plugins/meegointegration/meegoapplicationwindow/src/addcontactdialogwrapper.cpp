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

#include "addcontactdialogwrapper.h"
#include "quickaddcontactdialog.h"
#include <qdeclarative.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <qutim/servicemanager.h>
#include <QStringBuilder>


namespace MeegoIntegration
{
enum {
	AccountRole = Qt::UserRole,
	EnabledRole
};

Q_GLOBAL_STATIC(QList<AddContactDialogWrapper*>, m_managers)
QuickAddContactDialog* AddContactDialogWrapper::m_currentDialog;

AddContactDialogWrapper::AddContactDialogWrapper()
{
	m_managers()->append(this);
	m_accounts = new QList<Account*>();
	QHash<int, QByteArray> roleNames;
	roleNames.insert(AccountRole, "account");
	roleNames.insert(Qt::DisplayRole, "title");
	roleNames.insert(EnabledRole, "isEnabled");
	setRoleNames(roleNames);

}

void AddContactDialogWrapper::loadAccounts()
{
	if (m_accounts)
	{
		beginRemoveRows(QModelIndex(),0,m_accounts->size());
		m_accounts->clear();
		endRemoveRows();
	}
	else
		m_accounts = new QList<Account*>();

	foreach (Protocol *protocol, Protocol::all())
		if (protocol->data(Protocol::ProtocolContainsContacts).toBool() && !protocol->accounts().isEmpty())
			foreach (Account *acc, protocol->accounts()) {
				beginInsertRows(QModelIndex(), m_accounts->size(), m_accounts->size());
				m_accounts->append(acc);
				endInsertRows();
				changeState(acc, acc->status());
				connect(acc, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
					SLOT(changeState(qutim_sdk_0_3::Status)));
			}

	if (m_accounts->count() == 1)
	{
		Status s =m_accounts->at(0)->status();
		if (s != Status::Connecting && s != Status::Offline) {
			setAccount(m_accounts->at(0));
		}
		else
			m_showAccountsList = true;}
	else
		m_showAccountsList = true;

}

void AddContactDialogWrapper::changeState(const qutim_sdk_0_3::Status &status)
{
	if (Account *account = qobject_cast<Account *>(sender()))
		changeState(account, status);
}

void AddContactDialogWrapper::changeState(Account *account, const qutim_sdk_0_3::Status &status)
{
	Q_UNUSED(status)

	for (int i=0;i<m_accounts->size();i++)
	{
		Account *acc=m_accounts->at(i);
		if (account->id().compare(acc->id()) == 0)
		{
			emit dataChanged(index(i,0),index(i,0));
			break;
		}
	}
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
	for (int i=0;i<m_accounts->size();i++)
	{
		Account *acc=m_accounts->at(i);
		if (accountId.compare(acc->id()) == 0)
		{
			currentAccount = acc;
			break;
		}
	}
	m_showAccountsList = false;
	setContactIdLabel(accountId);
	emit showAccountsListChanged();
}
void AddContactDialogWrapper::setAccount(Account * account)
{
	currentAccount = account;
	setContactIdLabel(account->id());
	m_showAccountsList = false;
	emit showAccountsListChanged();
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
		m_managers()->at(i)->loadAccounts();
		emit m_managers()->at(i)->shown();
	}
}

int AddContactDialogWrapper::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_accounts->size();
}

QVariant AddContactDialogWrapper::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() > m_accounts->size())
		return QVariant();
	Account *account = m_accounts->at(index.row());
	switch (role) {
	case Qt::DisplayRole:
		return account->id();
	case Qt::DecorationRole:
		return QString();
	case AccountRole:
		return qVariantFromValue<QObject*>(account);
	case EnabledRole:{
		Status status = account->status();
		if (status == Status::Connecting || status == Status::Offline) {
			return false;
		}
		return true;}
	default:
		return QVariant();
	}
}

}

