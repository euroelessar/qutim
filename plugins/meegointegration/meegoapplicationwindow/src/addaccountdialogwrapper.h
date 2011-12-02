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

#ifndef ADDACCOUNTDIALOGWRAPPER_H
#define ADDACCOUNTDIALOGWRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "quickaddaccountdialog.h"
#include <QAbstractListModel>
#include <qutim/account.h>
#include <qutim/status.h>
#include <qutim/protocol.h>
#include <QWizard>
#include "quickwidgetproxy.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

class AddAccountDialogWrapper : public QAbstractListModel {
	Q_OBJECT

public:
	AddAccountDialogWrapper();
	~AddAccountDialogWrapper();

	Q_INVOKABLE void loadAccounts();
	Q_INVOKABLE QObject* getWidget(int index);
	Q_INVOKABLE bool validateWidget(QObject* widget);
	static void init();
	static void showDialog(QuickAddAccountDialog * dialog);

	// QAbstractListModel
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
signals:
	void shown();

private:
	static QuickAddAccountDialog *m_currentDialog;
	QWizard *m_wizard;
	QList<AccountCreationWizard*>* m_wizards;
	QMap<AccountCreationWizard *, int> m_wizardIds;


};
}

#endif /* ADDACCOUNTDIALOGWRAPPER_H */
