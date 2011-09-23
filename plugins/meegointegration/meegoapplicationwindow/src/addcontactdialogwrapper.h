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

#ifndef ADDCONTACTDIALOGWRAPPER_H
#define ADDCONTACTDIALOGWRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "quickaddcontactdialog.h"
#include <QAbstractListModel>
#include <qutim/account.h>
#include <qutim/status.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
class AddContactDialogWrapper : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(QString contactIdLabel READ contactIdLabel WRITE setContactIdLabel NOTIFY contactIdLabelChanged)
	Q_PROPERTY(bool showAccountsList READ showAccountsList NOTIFY showAccountsListChanged)
public:
	AddContactDialogWrapper();
	~AddContactDialogWrapper();

	QString contactIdLabel() const;
	bool showAccountsList() const;

	void setContactIdLabel(const QString &);
	Q_INVOKABLE void loadAccounts();

	Q_INVOKABLE void addContact(QString id, QString name);
	Q_INVOKABLE void setAccount(QString accountId);
	void setAccount(Account * account);

	static void init();
	static void showDialog(QuickAddContactDialog * dialog);

	// QAbstractListModel
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:
	void contactIdLabelChanged();
	void showAccountsListChanged();
	void shown();

private slots:
	void setAccount();

protected:


private:
	QString m_idLabel;
	static QuickAddContactDialog *m_currentDialog;
	QList<Account*> * m_accounts;
	Account * currentAccount;
	bool m_showAccountsList;


};
}

#endif /* ADDCONTACTDIALOGWRAPPER_H */
