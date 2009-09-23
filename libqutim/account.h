#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "configbase.h"

namespace qutim_sdk_0_3
{
	class Contact;
	class Protocol;
	struct AccountPrivate;

	class Account : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(Status status READ status WRITE setStatus)
	public:
		Account(const QString &id, Protocol *protocol);
		virtual ~Account();
		QString id();
		Config config();
		ConfigGroup config(const QString &group);
		Status status();
		virtual void setStatus(Status status);
	private:
		QScopedPointer<AccountPrivate> p;
	};
}

#endif // ACCOUNT_H
