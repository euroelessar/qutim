/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include "libqutim_global.h"
#include <QValidator>

namespace qutim_sdk_0_3
{
class Account;
class PasswordDialogPrivate;

class LIBQUTIM_EXPORT PasswordDialog : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(PasswordDialog)
public:
	enum DialogCode { Rejected, Accepted };
	
	static PasswordDialog *request(Account *account);
	static PasswordDialog *request(const QString &windowTitle, const QString &description);

	explicit PasswordDialog();
	virtual ~PasswordDialog();

	virtual void setValidator(QValidator *validator) = 0;

	QString login() const;
	QString password() const;
	bool remember() const;
	int exec();
	int result() const;
	void setSaveButtonVisible(bool allow);
	void setLoginEditVisible(bool allow);

signals:
	void rejected();
	void entered(const QString &password, bool remember);
	void finished(int result);

protected:
	void apply(const QString &login, const QString &password, bool remember);
	void apply(const QString &password, bool remember);
	void reject();
	
	enum PasswordDialogHook {
		SetTextHook = 1,
		SetShowSaveHook,
		SetShowLoginHook
	};
	
	struct SetTextArgument
	{
		QString title;
		QString description;
	};

	virtual void setAccount(Account *account) = 0;
	virtual void virtual_hook(int id, void *data);

	QScopedPointer<PasswordDialogPrivate> d_ptr;
};
}

#endif // PASSWORDDIALOG_H

