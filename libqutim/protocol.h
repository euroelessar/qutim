#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "configbase.h"

namespace qutim_sdk_0_3
{
	class Contact;
	class Account;
	struct ProtocolPrivate;

	class Protocol : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
	public:
		Protocol();
		virtual ~Protocol();
		Config config();
		ConfigGroup config(const QString &group);
		QString id();
	private:
		virtual void loadAccounts() = 0;
		friend class ModuleManager;
		QScopedPointer<ProtocolPrivate> p;
	};
}

#endif // PROTOCOL_H
