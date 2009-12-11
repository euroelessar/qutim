#ifndef JPROTOCOL_H
#define JPROTOCOL_H

#include <qutim/protocol.h>

using namespace qutim_sdk_0_3;

namespace Jabber
{
	class JProtocol : public QObject
	{
		Q_OBJECT
	public:
		JProtocol();

		static inline JProtocol *instance() { if(!self) qWarning("JProtocol isn't created"); return self; }
		virtual AccountCreationWizard *accountCreationWizard();
		virtual QList<Account *> accounts() const;
		virtual Account *account(const QString &id) const;
	private:
		static JProtocol *self;
	};
} // Jabber namespace

#endif // JPROTOCOL_H
