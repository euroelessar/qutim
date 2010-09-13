/****************************************************************************
 *  accountsmodel.cpp
 *
 *  Copyright (c) 2010 by Alexey Prokhin <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "accountsmodel.h"
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <algorithm>

namespace Core {

	AccountsModel::AccountsModel(QObject *parent) :
		QAbstractListModel(parent)
	{
		foreach (Protocol *protocol, allProtocols()) {
			// TODO: use Event instead of Protocol::data()
			bool support = protocol->data(qutim_sdk_0_3::Protocol::ProtocolSupportGroupChat).toBool();
			if (!support)
				continue;
			foreach (Account *account, protocol->accounts())
				onAccountCreated(account);
			connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
					SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		}
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

	void AccountsModel::onAccountCreated(qutim_sdk_0_3::Account *account)
	{
		connect(account, SIGNAL(nameChanged(QString,QString)),
				this, SLOT(onAccountNameChanged()));
		connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
				this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
		connect(account, SIGNAL(destroyed(QObject*)),
				this, SLOT(onAccountDestroyed(QObject*)));
		if (isActive(account))
			addAccount(account);
	}

	void AccountsModel::onAccountDestroyed(QObject *account)
	{
		removeAccount(static_cast<Account*>(account), false);
	}

	void AccountsModel::onAccountNameChanged()
	{
		Q_ASSERT(qobject_cast<Account*>(sender()));
		Account *account = static_cast<Account*>(sender());
		if (!isActive(account))
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

	void AccountsModel::onAccountStatusChanged(const qutim_sdk_0_3::Status &current,
											   const qutim_sdk_0_3::Status &previous)
	{
		Q_ASSERT(qobject_cast<Account*>(sender()));
		Account *account = static_cast<Account*>(sender());
		bool isActive = previous != Status::Offline;
		if (isActive && current == Status::Offline) {
			removeAccount(account, true);
		} else if (!isActive && current != Status::Offline) {
			addAccount(account);
		} else {
			int pos = m_accounts.indexOf(account);
			if (pos != 0) {
				QModelIndex i = index(pos, 0);
				emit dataChanged(i, i);
			}
		}
	}

	bool AccountsModel::isActive(Account *account) const
	{
		return account->status() != Status::Offline;
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
	}

} // namespace Core
