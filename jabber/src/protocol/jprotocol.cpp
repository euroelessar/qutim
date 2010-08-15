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
#include "account/muc/jmucmanager.h"
#include "account/muc/jbookmarkmanager.h"
#include "account/roster/jmessagesession.h"
#include <QInputDialog>

namespace Jabber
{

	enum JMUCActionType
	{
		JoinLeaveAction,
		SaveRemoveBookmarkAction,
		RoomConfigAction,
		RoomParticipantsAction
	};

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
		self = 0;
	}

	QList<Account *> JProtocol::accounts() const
	{
		QList<Account *> accounts;
		foreach (JAccount *account, p->accounts->values())
			accounts.append(account);
		return accounts;
	}

	Account *JProtocol::account(const QString &id) const
	{
		return p->accounts->value(id);
	}

	void JProtocol::loadActions()
	{
		Settings::registerItem(new GeneralSettingsItem<JMainSettings>(
				Settings::Protocol,
				Icon("im-jabber"),
				QT_TRANSLATE_NOOP("Settings", "Jabber")));

		MenuController::addAction<JMUCUser>(
				new ActionGenerator(Icon("im-kick-user"), QT_TRANSLATE_NOOP("Conference", "Kick"),
									this, SLOT(onKickUser(QObject*))));
		MenuController::addAction<JMUCUser>(
				new ActionGenerator(Icon("im-ban-user"), QT_TRANSLATE_NOOP("Conference", "Ban"),
									this, SLOT(onBanUser(QObject*))));
		MenuController::addAction<JMessageSession>(
				new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Conference", "Convert to conference"),
									this, SLOT(onConvertToMuc(QObject*))));
		
		ActionGenerator *generator  = new ActionGenerator(Icon(""),QT_TRANSLATE_NOOP("Jabber", "Join conference"),
				this, SLOT(onJoinLeave(QObject*)));
		generator->addHandler(ActionVisibilityChangedHandler,this);
		generator->setType(0);
		generator->setPriority(3);
		generator->addProperty("actionType",JoinLeaveAction);
		MenuController::addAction<JMUCSession>(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Room's configuration"),
				this, SLOT(onShowConfigDialog(QObject*)));
		generator->addHandler(ActionVisibilityChangedHandler,this);
		generator->setType(1);
		generator->addProperty("actionType",RoomConfigAction);
		MenuController::addAction<JMUCSession>(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks") ,
				this, SLOT(onSaveRemoveBookmarks(QObject*)));
		generator->addHandler(ActionVisibilityChangedHandler,this);
		generator->setType(0);
		generator->setPriority(0);
		generator->addProperty("actionType",SaveRemoveBookmarkAction);
		MenuController::addAction<JMUCSession>(generator);

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

	void JProtocol::onKickUser(QObject *obj)
	{
		JMUCUser *user = qobject_cast<JMUCUser*>(obj);
		Q_ASSERT(user);
		JMUCSession *muc = static_cast<JMUCSession *>(user->upperUnit());
		QString reason = QInputDialog::getText(0, tr("Kick"), tr("Enter kick reason for %1").arg(user->name()));
		muc->room()->kick(user->name().toStdString(), reason.toStdString());
	}

	void JProtocol::onBanUser(QObject *obj)
	{
		JMUCUser *user = qobject_cast<JMUCUser*>(obj);
		Q_ASSERT(user);
		JMUCSession *muc = static_cast<JMUCSession *>(user->upperUnit());
		QString reason = QInputDialog::getText(0, tr("Ban"), tr("Enter ban reason for %1").arg(user->name()));
		muc->room()->ban(user->name().toStdString(), reason.toStdString());
	}
	
	void JProtocol::onConvertToMuc(QObject *obj)
	{
		JMessageSession *session = qobject_cast<JMessageSession*>(obj);
		Q_ASSERT(session);
		session->convertToMuc();
	}
		
	void JProtocol::onJoinLeave(QObject* obj)
	{
		JMUCSession *room = qobject_cast<JMUCSession*>(obj);
		Q_ASSERT(room);
		if (!room->isJoined()) {
			JAccount *account = static_cast<JAccount*>(room->account());
			account->conferenceManager()->join(room->id());
		}
		else
			room->leave();
	}

	void JProtocol::onShowConfigDialog(QObject* obj)
	{
		JMUCSession *room = qobject_cast<JMUCSession*>(obj);
		Q_ASSERT(room);
		if (room->enabledConfiguring())
			room->showConfigDialog();
	}

	void JProtocol::onSaveRemoveBookmarks(QObject *obj)
	{
		//TODO move to joingroupchat module
		JMUCSession *room = qobject_cast<JMUCSession*>(obj);
		Q_ASSERT(room);
		JAccount *account = static_cast<JAccount*>(room->account());
		//FIXME,  WTF room->bookmarkIndex() == -1?
		if (room->bookmarkIndex() == -1)
			//Rewrite on event system, something like
			//Event event("groupchat-join-request");
			//QObject *obj = getService("JoinGroupChat");
			//qApp->sendEvent(obj,&event);
			account->conferenceManager();
		else
			account->conferenceManager()->bookmarkManager()->removeBookmark(room->bookmarkIndex());
	}

	void JProtocol::loadAccounts()
	{
		loadActions();
		QStringList accounts = config("general").value("accounts", QStringList());
		foreach(const QString &id, accounts) {
			JID jid(id.toStdString());
			addAccount(new JAccount(QString::fromStdString(jid.bare())), true);
		}
	}

	void JProtocol::addAccount(JAccount *account, bool isEmit)
	{
		p->accounts->insert(account->id(), account);
		if (isEmit)
			emit accountCreated(account);
	}
	
	QVariant JProtocol::data(DataType type)
	{
		switch (type) {
			case ProtocolIdName:
				return "Jabber ID";
			case ProtocolContainsContacts:
				return true;
			case ProtocolSupportGroupChat:
				return true;
			default:
				return QVariant();
		}
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

	bool JProtocol::event(QEvent *ev)
	{
		if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
			ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
			QAction *action = event->action();
			JMUCSession *room = qobject_cast<JMUCSession*>(event->controller());
			Q_ASSERT(room);
			JMUCActionType type = static_cast<JMUCActionType>(action->property("actionType").toInt());
			if (event->isVisible()) {
				switch (type) {
				case JoinLeaveAction: {
					if (!room->isJoined())
						action->setText(QT_TRANSLATE_NOOP("Jabber", "Join conference"));
					else
						action->setText(QT_TRANSLATE_NOOP("Jabber", "Leave conference"));
					break;
					}
				case RoomConfigAction: {
					action->setVisible(room->enabledConfiguring());
					break;
				}
				case SaveRemoveBookmarkAction: {
					if (room->bookmarkIndex() == -1)
						action->setText(QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks"));
					else
						action->setText(QT_TRANSLATE_NOOP("Jabber", "Remove from bookmarks"));
					break;
				}
				default:
					break;
				}
			}
		}
		return QObject::event(ev);
	}
}
