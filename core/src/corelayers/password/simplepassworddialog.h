#ifndef SIMPLEPASSWORDDIALOG_H
#define SIMPLEPASSWORDDIALOG_H

#include <qutim/passworddialog.h>
#include "simplepasswordwidget.h"

namespace Core
{
	using namespace qutim_sdk_0_3;

	class SimplePasswordDialog : public PasswordDialog
	{
		Q_OBJECT
	public:
		explicit SimplePasswordDialog();
		virtual void setAccount(Account *account);
		virtual void setValidator(QValidator *validator);
	private:
		QWeakPointer<SimplePasswordWidget> m_widget;
		friend class SimplePasswordWidget;
	};
}

#endif // SIMPLEPASSWORDDIALOG_H
