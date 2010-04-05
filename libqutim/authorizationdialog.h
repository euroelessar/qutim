#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class Contact;
	class AuthorizationDialogPrivate;

	class LIBQUTIM_EXPORT AuthorizationDialog : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "AuthorizationDialog")
		Q_DISABLE_COPY(AuthorizationDialog)
	public:
		virtual ~AuthorizationDialog();
		static AuthorizationDialog *request(Contact *contact, const QString &text, bool incoming = true);
		Contact *contact() const;
		virtual QString text() const = 0;
	protected:
		explicit AuthorizationDialog();
		virtual void setContact(Contact *contact, const QString &text, bool incoming = true) = 0;
		virtual void virtual_hook(int id, void *data);
		QScopedPointer<AuthorizationDialogPrivate> d;
	signals:
		void accepted();
		void rejected();
		void finished(bool ok);
	};
}

#endif // AUTHORIZATIONDIALOG_H
