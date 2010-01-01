#include "jprotocol.h"
#include "account/jaccount.h"
#include <qutim/icon.h>

#include <gloox/dataform.h>
#include "account/dataform/jdataform.h"

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

		gloox::DataForm *form = new gloox::DataForm(gloox::TypeForm, "cool title");
		form->addField(DataFormField::TypeTextSingle, "text-single", "uau, value", "cool label");
		{
			DataFormField *field = new DataFormField(DataFormField::TypeListSingle);
			field->setLabel("hello! choose me!");
			field->addOption("1231", "1231");
			field->addOption("1232", "1232");
			field->addOption("1233", "1233");
			form->addField(field);
		}
		JDataForm *jForm = new JDataForm(form, false);
		jForm->show();
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
		foreach(const QString &id, accounts) {
			JID jid(id.toStdString());
			addAccount(new JAccount(QString::fromStdString(jid.bare())));
		}
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
			account->beginChangeStatus(
					statusToPresence(static_cast<Status>(action->property("status").toInt())));
		}
	}

	Presence::PresenceType JProtocol::statusToPresence(Status status)
	{
		Presence::PresenceType presence;
		switch (status) {
		case Offline:
			presence = Presence::Unavailable;
			break;
		case Online:
			presence = Presence::Available;
			break;
		case Away:
			presence = Presence::Away;
			break;
		case FreeChat:
			presence = Presence::Chat;
			break;
		case DND:
			presence = Presence::DND;
			break;
		case NA:
			presence = Presence::XA;
			break;
		default:
			presence = Presence::Invalid;
		}
		return presence;
	}

	Status JProtocol::presenceToStatus(Presence::PresenceType presence)
	{
		Status status;
		switch (status) {
		case Presence::Available:
			status = Online;
			break;
		case Presence::Away:
			status = Away;
			break;
		case Presence::Chat:
			status = FreeChat;
			break;
		case Presence::DND:
			status = DND;
			break;
		case Presence::XA:
			status = NA;
			break;
		case Presence::Unavailable:
		default:
			status = Offline;
		}
		return status;
	}
}
