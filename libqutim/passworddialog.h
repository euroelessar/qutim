#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class Account;

	class LIBQUTIM_EXPORT PasswordDialog : public QObject
	{
		Q_OBJECT
	public:
		static PasswordDialog *request(Account *account);
	protected:
		explicit PasswordDialog();
		virtual void setAccount(Account *account) = 0;
		virtual void virtual_hook(int id, void *data);
	signals:
		void rejected();
		void entered(const QString &password, bool remember);
	};
}

#endif // PASSWORDDIALOG_H
