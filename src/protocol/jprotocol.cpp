#include "jprotocol.h"
#include <qutim/icon.h>

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

	void JProtocol::loadActions()
	{
		MenuController::addAction((new ActionGenerator(Icon("user-online-jabber"),
			QT_TRANSLATE_NOOP("Status", "Online"),
			this, SLOT(onStatusActionPressed())))->addProperty("status", Online)->setPriority(Online),
			&JAccount::staticMetaObject);

		MenuController::addAction((new ActionGenerator(Icon("user-online-chat-jabber"),
			QT_TRANSLATE_NOOP("Status", "Free for chat"),
			this, SLOT(onStatusActionPressed())))->addProperty("status", FreeChat)->setPriority(FreeChat),
			&JAccount::staticMetaObject);

		MenuController::addAction((new ActionGenerator(Icon("user-away-jabber"),
			QT_TRANSLATE_NOOP("Status", "Away"),
			this, SLOT(onStatusActionPressed())))->addProperty("status", Away)->setPriority(Away),
			&JAccount::staticMetaObject);

		MenuController::addAction((new ActionGenerator(Icon("user-away-extended-jabber"),
			QT_TRANSLATE_NOOP("Status", "NA"),
			this, SLOT(onStatusActionPressed())))->addProperty("status", NA)->setPriority(NA),
			&JAccount::staticMetaObject);

		MenuController::addAction((new ActionGenerator(Icon("user-busy-jabber"),
			QT_TRANSLATE_NOOP("Status", "DND"),
			this, SLOT(onStatusActionPressed())))->addProperty("status", DND)->setPriority(DND),
			&JAccount::staticMetaObject);

		MenuController::addAction((new ActionGenerator(Icon("user-offline-jabber"),
			QT_TRANSLATE_NOOP("Status", "Offline"),
			this, SLOT(onStatusActionPressed())))->addProperty("status", Offline)->setPriority(Offline),
			&JAccount::staticMetaObject);
	}

	void JProtocol::loadAccounts()
	{
		loadActions();

		QStringList accounts = config("general").value("accounts", QStringList());
		foreach(const QString &jid, accounts)
			addAccount(new JAccount(jid));
	}

	void JProtocol::addAccount(JAccount *account, bool isEmit)
	{
		p->accounts->insert(account->id(), account);
		if (isEmit)
			emit accountCreated(account);
	}

	void JProtocol::onStatusActionPressed()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		MenuController *item = action->data().value<MenuController *>();
		if (JAccount *account = qobject_cast<JAccount *>(item)) {
			account->setStatus(static_cast<Status>(action->property("status").toInt()));
		}
	}
}
