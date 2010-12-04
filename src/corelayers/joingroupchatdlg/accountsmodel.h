/****************************************************************************
 *  accountsmodel.h
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

#ifndef ACCOUNTSMODEL_H
#define ACCOUNTSMODEL_H

#include <QAbstractListModel>
#include <qutim/status.h>

namespace qutim_sdk_0_3 {
class Account;
}

namespace Core {

using namespace qutim_sdk_0_3;

class AccountsModel : public QAbstractListModel
{
	Q_OBJECT
public:
	AccountsModel(QObject *parent = 0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private slots:
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountDestroyed(QObject *account);
	void onAccountNameChanged();
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &current,
								const qutim_sdk_0_3::Status &previous);
private:
	bool isActive(Account *account) const;
	QString title(Account *account) const;
	int findPlaceForAccount(Account *account) const;
	void addAccount(Account *account);
	void removeAccount(Account *account, bool disconnectSlots);
	QList<Account*> m_accounts;
};

} // namespace Core

#endif // ACCOUNTSMODEL_H
