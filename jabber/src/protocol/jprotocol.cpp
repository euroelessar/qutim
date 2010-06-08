#include "jprotocol.h"
#include "jmainsettings.h"
#include "account/jaccount.h"
#include <qutim/icon.h>
#include "account/roster/jresourceactiongenerator.h"
#include "account/roster/jcontact.h"
#include <gloox/dataform.h>
#include "account/dataform/jdataform.h"
#include <qutim/statusactiongenerator.h>
#include <qutim/settingslayer.h>
#include "account/muc/jmucuser.h"
#include "account/muc/jmucsession.h"
#include <QInputDialog>

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
		Settings::registerItem(new GeneralSettingsItem<JMainSettings>(
				Settings::Protocol,
				Icon("im-jabber"),
				QT_TRANSLATE_NOOP("Settings", "Jabber")));

		MenuController::addAction<JMUCUser>(
				new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Conference", "Kick"),
									this, SLOT(onKickUser())));
		MenuController::addAction<JMUCUser>(
				new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Conference", "Ban"),
									this, SLOT(onBanUser())));

		QList<Status> statuses;
		statuses << Status(Status::Online)
				 << Status(Status::FreeChat)
				 << Status(Status::Away)
				 << Status(Status::NA)
				 << Status(Status::DND)
				 << Status(Status::Offline);

		foreach (Status status, statuses) {
			status.initIcon("jabber");
			Status::remember(status, "jabber");
			MenuController::addAction(new StatusActionGenerator(status), &JAccount::staticMetaObject);
		}
	}

	void JProtocol::onKickUser()
	{
		JMUCUser *user = MenuController::getController<JMUCUser>(sender());
		JMUCSession *muc = static_cast<JMUCSession *>(user->upperUnit());
		QString reason = QInputDialog::getText(0, tr("Kick"), tr("Enter kick reason for %1").arg(user->name()));
		muc->room()->kick(user->name().toStdString(), reason.toStdString());
	}

	void JProtocol::onBanUser()
	{
		JMUCUser *user = MenuController::getController<JMUCUser>(sender());
		JMUCSession *muc = static_cast<JMUCSession *>(user->upperUnit());
		QString reason = QInputDialog::getText(0, tr("Ban"), tr("Enter ban reason for %1").arg(user->name()));
		muc->room()->ban(user->name().toStdString(), reason.toStdString());
	}

	void JProtocol::loadAccounts()
	{
		loadActions();
		QStringList accounts = config("general").value("accounts", QStringList());
		foreach(const QString &id, accounts) {
			JID jid(id.toStdString());
			addAccount(new JAccount(QString::fromStdString(jid.bare())), true);
		}

		/*MenuController::addAction<JContact>(
				new JResourceActionGenerator(QIcon(),
											 QT_TRANSLATE_NOOP("Test", "Test action"),
											 this, SLOT(onStatusActionPressed())));
		gloox::DataForm *form = new gloox::DataForm(gloox::TypeForm, "cool title");
		form->addField(DataFormField::TypeTextSingle, "text-single", "uau, value", "cool label");
		form->addField(DataFormField::TypeJidSingle, "jid-single", "test@qutim.org", "jid label");
		{
			DataFormField *field = new DataFormField(DataFormField::TypeListSingle);
			field->setName("list-single");
			field->setLabel("hello! choose me!");
			field->addOption("1231", "1231");
			field->addOption("1232", "1232");
			field->addOption("1233", "1233");
			form->addField(field);
		}
		{
			DataFormField *field = new DataFormField(DataFormField::TypeListMulti);
			field->setName("list-multi");
			field->setLabel("hello! this is List!");
			field->addOption("101", "1231");
			field->addOption("102", "1232");
			field->addOption("103", "1233");
			form->addField(field);
		}
		{
			DataFormField *field = new DataFormField(DataFormField::TypeJidMulti);
			field->setName("jid-multi");
			field->setLabel("JID List!");
			field->addValue("test@qutim.org");
			field->addValue("test2@qutim.org");
			field->addValue("test3@qutim.org");
			form->addField(field);
		}
		{
			DataFormField *field = new DataFormField(DataFormField::TypeFixed);
			field->setName("fixed");
			field->addValue("hello! this is long, very long Text for test!");
			form->addField(field);
		}
		{
			DataFormField *field = new DataFormField(DataFormField::TypeBoolean);
			field->setName("boolean");
			field->setLabel("hello! this is long caption for boolean widget!");
			form->addField(field);
		}
		JDataForm *jForm = new JDataForm(form, false);
		jForm->show();*/
	}

	void JProtocol::addAccount(JAccount *account, bool isEmit)
	{
		p->accounts->insert(account->id(), account);
		if (isEmit)
			emit accountCreated(account);
	}

	Presence::PresenceType JProtocol::statusToPresence(const Status &status)
	{
		Presence::PresenceType presence;
		switch (status.type()) {
		case Status::Offline:
			presence = Presence::Unavailable;
			break;
		case Status::Online:
			presence = Presence::Available;
			break;
		case Status::Away:
			presence = Presence::Away;
			break;
		case Status::FreeChat:
			presence = Presence::Chat;
			break;
		case Status::DND:
			presence = Presence::DND;
			break;
		case Status::NA:
			presence = Presence::XA;
			break;
		default:
			presence = Presence::Invalid;
		}
		return presence;
	}

	Status JProtocol::presenceToStatus(Presence::PresenceType presence)
	{
		Status::Type status;
		switch (presence) {
		case Presence::Available:
			status = Status::Online;
			break;
		case Presence::Away:
			status = Status::Away;
			break;
		case Presence::Chat:
			status = Status::FreeChat;
			break;
		case Presence::DND:
			status = Status::DND;
			break;
		case Presence::XA:
			status = Status::NA;
			break;
		case Presence::Unavailable:
		default:
			status = Status::Offline;
		}
		return Status::instance(status, "jabber");
	}
}
