/****************************************************************************
 *  passworddialog.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "passworddialog.h"
#include "account.h"
#include <QEventLoop>

namespace qutim_sdk_0_3
{
	Q_GLOBAL_STATIC_WITH_ARGS(const ObjectGenerator*, data, (NULL))

	PasswordDialog *PasswordDialog::request(Account *account)
	{
		const ObjectGenerator * &gen = *data();
		if (!gen) {
			const GeneratorList list = ObjectGenerator::module<PasswordDialog>();
			Q_ASSERT(!list.isEmpty());
			if (list.isEmpty())
				return NULL;
			gen = *list.begin();
		}
		PasswordDialog *dialog = gen->generate<PasswordDialog>();
		dialog->setAccount(account);
		return dialog;
	}

	struct PasswordDialogPrivate
	{
		QString password;
		bool remember;
		int result;
		QEventLoop *eventLoop;
	};

	PasswordDialog::PasswordDialog() : d_ptr(new PasswordDialogPrivate)
	{
		Q_D(PasswordDialog);
		d->remember = false;
		d->result = Rejected;
		d->eventLoop = 0;
	}

	PasswordDialog::~PasswordDialog()
	{
	}

	QString PasswordDialog::password() const
	{
		return d_func()->password;
	}

	bool PasswordDialog::remember() const
	{
		return d_func()->remember;
	}

	void PasswordDialog::apply(const QString &password, bool remember)
	{
		Q_D(PasswordDialog);
		d->password = password;
		d->remember = remember;
		d->result = Accepted;
		if (d->eventLoop)
			d->eventLoop->quit();
		emit entered(password, remember);
	}

	void PasswordDialog::reject()
	{
		Q_D(PasswordDialog);
		d->password.clear();
		d->remember = false;
		d->result = Rejected;
		if (d->eventLoop)
			d->eventLoop->quit();
		emit rejected();
	}

	int PasswordDialog::exec()
	{
		Q_D(PasswordDialog);
		if (d->eventLoop) // recursive call
			return -1;
		QEventLoop eventLoop;
		d->eventLoop = &eventLoop;
		QPointer<PasswordDialog> guard = this;
		(void) eventLoop.exec();
		d->eventLoop = 0;
		if (guard.isNull())
			return PasswordDialog::Rejected;
		return result();
	}

	int PasswordDialog::result() const
	{
		return d_func()->result;
	}

	void PasswordDialog::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}
