/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "accountsmodel.h"
#include <qutim/accountmanager.h>
#include <qutim/groupchatmanager.h>
#include <algorithm>

namespace Core {

AccountsModel::AccountsModel(QObject *parent) :
	QAbstractListModel(parent)
{
	AccountManager *manager = AccountManager::instance();
	connect(manager, &AccountManager::accountAdded, this, &AccountsModel::onAccountCreated);
	connect(manager, &AccountManager::accountRemoved, this, &AccountsModel::onAccountDestroyed);

	foreach (Account *account, manager->accounts())
		onAccountCreated(account);
}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_accounts.size();
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
	Account *account = m_accounts.value(index.row());
	if (!account)
		return QVariant();
	if (role == Qt::DisplayRole)
		return title(account);
	else if (role == Qt::DecorationRole)
		return account->status().icon();
	else if (role == Qt::UserRole)
		return QVariant::fromValue(account);
	return QVariant();
}

void AccountsModel::onAccountCreated(Account *account)
{
	connect(account, &Account::nameChanged, this, &AccountsModel::onAccountNameChanged);
	connect(account, &Account::statusChanged, this, &AccountsModel::onAccountStatusChanged);
	connect(account, &Account::interfaceChanged, this, [this] (const QByteArray &name, QObject *interface) {
		if (name == "GroupChatManager") {
			onGroupChatManagerChanged(qobject_cast<GroupChatManager *>(interface));
		}
	});
	if (account->groupChatManager())
		addAccount(account);
}

void AccountsModel::onAccountDestroyed(Account *account)
{
	removeAccount(account, false);
}

void AccountsModel::onAccountNameChanged()
{
	Q_ASSERT(qobject_cast<Account*>(sender()));
	Account *account = static_cast<Account*>(sender());
	if (!account->groupChatManager())
		return;
	int oldPos = m_accounts.indexOf(account);
	if (oldPos == -1)
		return;
	int newPos = findPlaceForAccount(account);
	if (oldPos == newPos) {
		QModelIndex i = index(oldPos, 0);
		emit dataChanged(i, i);
	} else {
		if (beginMoveRows(QModelIndex(), oldPos, oldPos, QModelIndex(), newPos)) {
			if (oldPos < newPos)
				--newPos;
			m_accounts.move(oldPos, newPos);
			endMoveRows();
		}
	}
}

void AccountsModel::onGroupChatManagerChanged(qutim_sdk_0_3::GroupChatManager *manager)
{
	Q_ASSERT(qobject_cast<Account*>(sender()));
	Account *account = static_cast<Account*>(sender());
	if (manager)
		addAccount(account);
	else
		removeAccount(account, true);
}

void AccountsModel::onAccountStatusChanged(const qutim_sdk_0_3::Status &current,
										   const qutim_sdk_0_3::Status &previous)
{
	Q_UNUSED(current);
	Q_UNUSED(previous);
	Q_ASSERT(qobject_cast<Account*>(sender()));
	Account *account = static_cast<Account*>(sender());
	if (!account->groupChatManager())
		return;
	int pos = m_accounts.indexOf(account);
	if (pos != 0) {
		QModelIndex i = index(pos, 0);
		emit dataChanged(i, i);
	}
}

inline QString AccountsModel::title(Account *account) const
{
	return account->id();
}

int AccountsModel::findPlaceForAccount(Account *account) const
{
	QString accountTitle = title(account);
	int i = 0;
	foreach (Account *current, m_accounts) {
		if (QString::localeAwareCompare(title(current), accountTitle) > 0)
			return i;
		++i;
	}
	return i;
}

void AccountsModel::addAccount(Account *account)
{
	if (m_accounts.indexOf(account) != -1)
		return;
	int pos = findPlaceForAccount(account);
	beginInsertRows(QModelIndex(), pos, pos);
	m_accounts.insert(pos, account);
	endInsertRows();
}

void AccountsModel::removeAccount(Account *account, bool disconnectSlots)
{
	int pos = m_accounts.indexOf(account);
	if (pos == -1)
		return;
	beginRemoveRows(QModelIndex(), pos, pos);
	m_accounts.removeAt(pos);
	endRemoveRows();
	if(disconnectSlots)
		account->disconnect(this);
}

} // namespace Core

