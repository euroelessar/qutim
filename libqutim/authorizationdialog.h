#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class Contact;

	class LIBQUTIM_EXPORT AuthorizationDialog : public QObject
	{
		Q_OBJECT
	public:
		static AuthorizationDialog *request(Contact *contact, const QString &text);
	protected:
		explicit AuthorizationDialog();
		virtual void setContact(Contact *contact, const QString &text) = 0;
		virtual void virtual_hook(int id, void *data);
	signals:
		void accepted();
		void rejected();
	};
}

#endif // AUTHORIZATIONDIALOG_H
