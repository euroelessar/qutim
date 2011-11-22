/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef ADDCONTACT_H
#define ADDCONTACT_H
#include <QScopedPointer>
#include <QDialog>
#include <qutim/account.h>
#include <qutim/status.h>

namespace Core {
using namespace qutim_sdk_0_3;
class AddContactPrivate;

class AddContact : public QDialog
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(AddContact)
public:
	AddContact(Account *account = 0, QWidget *parent = 0);
	~AddContact();
	
	void setContactId(const QString &id);
	void setContactName(const QString &name);
	void setContactTags(const QStringList &tags);
protected:
	void setAccount(Account *account);
	void changeState(Account *account, const qutim_sdk_0_3::Status &status);
private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void onStartChatClicked();
	void onShowInfoClicked();
	void setAccount();
	void changeState(const qutim_sdk_0_3::Status &status);
	void currentChanged(int index);
private:
	QScopedPointer<AddContactPrivate> d_ptr;
};

class AddContactModule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "AddContact")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ContactList")
public:
	AddContactModule();
	~AddContactModule();
public slots:
	void show(qutim_sdk_0_3::Account *account, const QString &id = QString(),
	          const QString &name = QString(), const QStringList &tags = QStringList());
private slots:
	void show();
private:
	QScopedPointer<ActionGenerator> m_addUserGen;
};
}
#endif // ADDCONTACT_H

