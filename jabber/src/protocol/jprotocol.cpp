#include "jprotocol.h"
#include "jmainsettings.h"
#include "account/jaccount.h"
#include <qutim/icon.h>
#include "account/roster/jresourceactiongenerator.h"
#include "account/roster/jcontact.h"
//#include <gloox/dataform.h>
#include "account/dataform/jdataform.h"
#include <qutim/statusactiongenerator.h>
#include <qutim/settingslayer.h>
#include "account/muc/jmucuser.h"
#include "account/muc/jmucsession.h"
#include "account/muc/jmucmanager.h"
#include "account/muc/jbookmarkmanager.h"
#include "account/roster/jmessagesession.h"
#include <QInputDialog>
#include <qutim/debug.h>

namespace Jabber
{

enum JActionType
{
	JoinLeaveAction,
	SaveRemoveBookmarkAction,
	RoomConfigAction,
	RoomParticipantsAction,
	ChangeSubcriptionAction
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
	//MenuController::addAction<JMessageSession>(
	//			new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Conference", "Convert to conference"),
	//								this, SLOT(onConvertToMuc(QObject*))));

	ActionGenerator *generator  = new ActionGenerator(QIcon(),QT_TRANSLATE_NOOP("Jabber", "Join conference"),
													  this, SLOT(onJoinLeave(QObject*)));
	generator->addHandler(ActionVisibilityChangedHandler,this);
	generator->setType(0);
	generator->setPriority(3);
	generator->addProperty("actionType",JoinLeaveAction);
	MenuController::addAction<JMUCSession>(generator);

	generator = new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Jabber", "Room's configuration"),
									this, SLOT(onShowConfigDialog(QObject*)));
	generator->addHandler(ActionVisibilityChangedHandler,this);
	generator->setType(1);
	generator->addProperty("actionType",RoomConfigAction);
	MenuController::addAction<JMUCSession>(generator);

	generator = new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks") ,
									this, SLOT(onSaveRemoveBookmarks(QObject*)));
	generator->addHandler(ActionVisibilityChangedHandler,this);
	generator->setType(0);
	generator->setPriority(0);
	generator->addProperty("actionType",SaveRemoveBookmarkAction);
	MenuController::addAction<JMUCSession>(generator);

	generator = new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Jabber", "Change Subscription") ,
									this, SLOT(onChangeSubscription(QObject*)));
	generator->addHandler(ActionVisibilityChangedHandler,this);
	generator->setType(0);
	generator->setPriority(0);
	generator->addProperty("actionType",ChangeSubcriptionAction);
	MenuController::addAction<JContact>(generator);

	QList<Status> statuses;
	statuses << Status(Status::Online)
			 << Status(Status::FreeChat)
			 << Status(Status::Away)
			 << Status(Status::NA)
			 << Status(Status::DND)
			 << Status(Status::Invisible)
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
	muc->room()->kick(user->name(), reason);
}

void JProtocol::onBanUser(QObject *obj)
{
	JMUCUser *user = qobject_cast<JMUCUser*>(obj);
	Q_ASSERT(user);
	JMUCSession *muc = static_cast<JMUCSession *>(user->upperUnit());
	QString reason = QInputDialog::getText(0, tr("Ban"), tr("Enter ban reason for %1").arg(user->name()));
	muc->room()->ban(user->name(), reason);
}

void JProtocol::onConvertToMuc(QObject *obj)
{
	//JMessageSession *session = qobject_cast<JMessageSession*>(obj);
	//Q_ASSERT(session);
	//session->convertToMuc();
}

void JProtocol::onJoinLeave(QObject* obj)
{
	JMUCSession *room = qobject_cast<JMUCSession*>(obj);
	debug() << Q_FUNC_INFO << obj;
	Q_ASSERT(room);
	if (!room->isJoined()) {
//		JAccount *account = static_cast<JAccount*>(room->account());
		room->join();
//		account->conferenceManager()->join(room->id());
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
	JBookmarkManager *manager = account->conferenceManager()->bookmarkManager();
	if (!room->bookmark().isValid()) {
		QString name = room->id();
		manager->saveBookmark(-1,
							  name,
							  room->id(),
							  room->me()->name(),
							  QString());
	} else {
		manager->removeBookmark(room->bookmark());
		room->setBookmark(jreen::Bookmark::Conference());
	}
}

void JProtocol::onChangeSubscription(QObject *obj)
{
	JContact *contact = qobject_cast<JContact*>(obj);
	Q_ASSERT(contact);
	switch(contact->subscription()) {
	case jreen::AbstractRosterItem::Both:
	case jreen::AbstractRosterItem::To:
		contact->removeSubscription();
		break;
	case jreen::AbstractRosterItem::From:
	case jreen::AbstractRosterItem::None:
	case jreen::AbstractRosterItem::Invalid:
		contact->requestSubscription();
		break;
	default:
		break;
	}
}

void JProtocol::loadAccounts()
{
	loadActions();
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &id, accounts) {
		jreen::JID jid = id;
		addAccount(new JAccount(jid.bare()), true);
	}
}

void JProtocol::addAccount(JAccount *account, bool isEmit)
{
	p->accounts->insert(account->id(), account);
	if(isEmit)
		emit accountCreated(account);

	connect(account,SIGNAL(destroyed(QObject*)),SLOT(removeAccount(QObject*)));
}

QVariant JProtocol::data(DataType type)
{
	switch (type) {
	case ProtocolIdName:
		return "Jabber ID";
	case ProtocolContainsContacts:
		return true;
	default:
		return QVariant();
	}
}

jreen::Presence::Type JStatus::statusToPresence(const Status &status)
{
	jreen::Presence::Type presence;
	switch (status.type()) {
	case Status::Offline:
		presence = jreen::Presence::Unavailable;
		break;
	case Status::Online:
		presence = jreen::Presence::Available;
		break;
	case Status::Away:
		presence = jreen::Presence::Away;
		break;
	case Status::FreeChat:
		presence = jreen::Presence::Chat;
		break;
	case Status::DND:
		presence = jreen::Presence::DND;
		break;
	case Status::NA:
		presence = jreen::Presence::XA;
		break;
	case Status::Invisible:
		presence = jreen::Presence::XA;
		break;
	default:
		presence = jreen::Presence::Invalid;
	}
	return presence;
}

Status JStatus::presenceToStatus(jreen::Presence::Type presence)
{
	Status::Type status;
	switch (presence) {
	case jreen::Presence::Available:
		status = Status::Online;
		break;
	case jreen::Presence::Away:
		status = Status::Away;
		break;
	case jreen::Presence::Chat:
		status = Status::FreeChat;
		break;
	case jreen::Presence::DND:
		status = Status::DND;
		break;
	case jreen::Presence::XA:
		status = Status::NA;
		break;
	case jreen::Presence::Error:
	case jreen::Presence::Unavailable:
	default: //TODO probe,subscribe etc. isn't offline status
		status = Status::Offline;
	}
	return Status::instance(status, "jabber");
}

bool JProtocol::event(QEvent *ev)
{
	if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
		ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
		QAction *action = event->action();		
		JActionType type = static_cast<JActionType>(action->property("actionType").toInt());
		if (event->isVisible()) {
			switch (type) {
			case JoinLeaveAction: {
				Conference *room = qobject_cast<JMUCSession*>(event->controller());
				if (!room->isJoined())
					action->setText(QT_TRANSLATE_NOOP("Jabber", "Join conference"));
				else
					action->setText(QT_TRANSLATE_NOOP("Jabber", "Leave conference"));
				break;
			}
			case RoomConfigAction: {
				JMUCSession *room = qobject_cast<JMUCSession*>(event->controller());
				action->setVisible(room->enabledConfiguring());
				break;
			}
			case SaveRemoveBookmarkAction: {
				JMUCSession *room = qobject_cast<JMUCSession*>(event->controller());
				if (!room->bookmark().isValid())
					action->setText(QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks"));
				else
					action->setText(QT_TRANSLATE_NOOP("Jabber", "Remove from bookmarks"));
				break;
			}
			case ChangeSubcriptionAction: {
				JContact *contact = qobject_cast<JContact*>(event->controller());
				LocalizedString str;
				switch(contact->subscription()) {
				case jreen::AbstractRosterItem::Both:
				case jreen::AbstractRosterItem::To:
					str = QT_TRANSLATE_NOOP("Jabber", "Remove subscription");
					break;
				case jreen::AbstractRosterItem::From:
				case jreen::AbstractRosterItem::None:
				case jreen::AbstractRosterItem::Invalid:
					str = QT_TRANSLATE_NOOP("Jabber", "Request subscription");
					break;
				default:
					break;
				}
				action->setText(str);
			}
			default:
				break;
			}
		}
	} else if (ev->type() == ExtendedInfosEvent::eventType()) {
		ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(ev);
		QVariantHash clientInfo;
		clientInfo.insert("id", "client");
		clientInfo.insert("name", tr("Possible client"));
		clientInfo.insert("settingsDescription", tr("Show client icon"));
		event->addInfo("client", clientInfo);
		return true;
	}
	return QObject::event(ev);
}

void JProtocol::removeAccount(QObject *obj)
{
	JAccount *acc = reinterpret_cast<JAccount*>(obj);
	p->accounts->remove(p->accounts->key(acc));
}
}
