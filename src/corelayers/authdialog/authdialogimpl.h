#ifndef AUTHDIALOGIMPL_H
#define AUTHDIALOGIMPL_H
#include <libqutim/authorizationdialog.h>
#include <QPointer>

namespace Core {

	using namespace qutim_sdk_0_3;
	
	class AuthDialogPrivate;
	class AuthDialogImpl : public AuthorizationDialog
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "AuthorizationDialog")
	public:
		AuthDialogImpl();
		virtual void setContact(Contact* contact, const QString& text);
		virtual ~AuthDialogImpl();
	private:
		QPointer<AuthDialogPrivate> p;
	};

}
#endif // AUTHDIALOGIMPL_H
