#ifndef SIMPLEPASSWORDDIALOG_H
#define SIMPLEPASSWORDDIALOG_H

#include "libqutim/passworddialog.h"

namespace Core
{
	using namespace qutim_sdk_0_3;

	class SimplePasswordDialog : public PasswordDialog
	{
		Q_OBJECT
	public:
		explicit SimplePasswordDialog();
		virtual void setAccount(Account *account);
	};
}

#endif // SIMPLEPASSWORDDIALOG_H
