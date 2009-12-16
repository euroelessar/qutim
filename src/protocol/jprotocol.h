#ifndef JPROTOCOL_H
#define JPROTOCOL_H

#include <qutim/protocol.h>
#include "account/jaccount.h"

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	struct JProtocolPrivate;
	class JAccount;

	class JProtocol : public Protocol
	{
		Q_OBJECT
		Q_CLASSINFO("Protocol", "jabber")
		public:
			JProtocol();
			virtual ~JProtocol();
			static inline JProtocol *instance() {
				if(!self) qWarning("JProtocol isn't created");
				return self;
			}
			virtual QList<Account *> accounts() const;
			virtual Account *account(const QString &id) const;
			void addAccount(JAccount *account, bool isEmit = false);
		private:
			virtual void loadAccounts();
			static JProtocol *self;
		QScopedPointer<JProtocolPrivate> p;
	};
} // Jabber namespace

#endif // JPROTOCOL_H
