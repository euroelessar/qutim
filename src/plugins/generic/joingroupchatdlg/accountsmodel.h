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

#ifndef ACCOUNTSMODEL_H
#define ACCOUNTSMODEL_H

#include <QAbstractListModel>
#include <qutim/status.h>

namespace qutim_sdk_0_3 {
class Account;
class GroupChatManager;
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
	void onAccountCreated(Account *account);
	void onAccountDestroyed(Account *account);
	void onAccountNameChanged();
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &current,
								const qutim_sdk_0_3::Status &previous);
	void onGroupChatManagerChanged(qutim_sdk_0_3::GroupChatManager *manager);
private:
	QString title(Account *account) const;
	int findPlaceForAccount(Account *account) const;
	void addAccount(Account *account);
	void removeAccount(Account *account, bool disconnectSlots);
	QList<Account*> m_accounts;
};

} // namespace Core

#endif // ACCOUNTSMODEL_H

