#include "jprotocol.h"

namespace Jabber
{
	struct JProtocolPrivate
	{
		inline JProtocolPrivate() : accounts(new QHash<QString, JAccount *>) {}
		inline ~JProtocolPrivate() {delete accounts;}
		QHash<QString, JAccount *> *accounts;
	};

	JProtocol *JProtocol::self = 0;

	JProtocol::JProtocol() : p(new JProtocolPrivate)
	{
		Q_ASSERT(!self);
		self = this;
		//p = new JProtocolPrivate();
	}

	JProtocol::~JProtocol()
	{
	}

	QList<Account *> JProtocol::accounts() const
	{
		QList<Account *> accounts;
		foreach (JAccount *account, p->accounts->values())
			accounts.append(qobject_cast<Account *>(account));
		return accounts;
	}

	Account *JProtocol::account(const QString &id) const
	{
		return qobject_cast<Account *>(p->accounts->value(id));
	}

	void JProtocol::loadAccounts()
	{
		QStringList accounts = config("general").value("accounts", QStringList());
		foreach(const QString &jid, accounts)
			p->accounts->insert(jid, new JAccount(jid));

	}
}
