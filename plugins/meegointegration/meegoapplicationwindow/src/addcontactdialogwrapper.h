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
namespace MeegoIntegration
{

class AddContactDialogWrapper : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString contactIdLabel READ contactIdLabel WRITE setContactIdLabel NOTIFY contactIdLabelChanged)
	Q_PROPERTY(QString contactIdText READ contactIdText WRITE contactIdText NOTIFY contactIdTextChanged)
	Q_PROPERTY(QString contactNameText READ contactNameText WRITE setContactNameText NOTIFY contactNameTextChanged)
public:
	AddContactDialogWrapper();
	~AddContactDialogWrapper();
	QString contactIdLabel() const;
	QString contactIdText() const;
	QString contactNameText() const;

	void setContactIdLabel(const QString &);
	void setContactIdText(const QString &);
	void setcontactNameText(const QString &);

	Q_INVOKABLE void accept();
	Q_INVOKABLE void cancel();

	static void init();
	static void showDialog(QString title, QuickAddContactDialog * passDialog);

signals:
	void contactIdLabelChanged();
	void contactIdTextChanged();
	void contactNameTextChanged();
	void shown();

private:
	QString m_idLabel;
	QString m_idText;
	QString m_nameText;
	static QuickAddContactDialog *m_currentDialog;

protected:
	void setAccount(Account *account);
	void changeState(Account *account, const qutim_sdk_0_3::Status &status);
private slots:
	void setAccount();
	void changeState(const qutim_sdk_0_3::Status &status);
	void currentChanged(int index);

};
}

#endif /* ADDCONTACTDIALOGWRAPPER_H */
