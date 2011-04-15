/****************************************************************************
 *  passworddialog.h
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

		explicit PasswordDialog();
		virtual ~PasswordDialog();

		virtual void setValidator(QValidator *validator) = 0;

		QString password() const;
		bool remember() const;
		int exec();
		int result() const;

	signals:
#if !defined(Q_MOC_RUN) && !defined(DOXYGEN_SHOULD_SKIP_THIS) && !defined(IN_IDE_PARSER)
private: // don't tell moc, doxygen or kdevelop, but those signals are in fact private
#endif
		void rejected();
		void entered(const QString &password, bool remember);
		void finished(int result);

	protected:
		void apply(const QString &password, bool remember);
		void reject();

		virtual void setAccount(Account *account) = 0;
		virtual void virtual_hook(int id, void *data);

		QScopedPointer<PasswordDialogPrivate> d_ptr;
	};
}

#endif // PASSWORDDIALOG_H
