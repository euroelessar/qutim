#ifndef CHATUNIT_H
#define CHATUNIT_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class Account;
	class Message;
	class ChatUnit;
	class ChatUnitPrivate;
	typedef QList<ChatUnit *> ChatUnitList;

	class ChatUnit : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(Account* account READ account)
	public:
		ChatUnit(Account *account);
		ChatUnit(Account *account, ChatUnitPrivate *d);
		virtual ~ChatUnit();
		virtual QString id() const = 0;
		Account *account();
		const Account *account() const;
		virtual void sendMessage(const Message &message) = 0;
		virtual ChatUnitList lowerUnits();
		virtual ChatUnit *upperUnit();
	protected:
		QScopedPointer<ChatUnitPrivate> p;
	};
}

#endif // CHATUNIT_H
