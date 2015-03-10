/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2014 Nicolay Izoderov <nico-izo@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "jprotocol.h"
#include "jmainsettings.h"
#include "account/jaccount.h"
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include "account/roster/jresourceactiongenerator.h"
#include "account/roster/jcontact.h"
#include "account/dataform/jdataform.h"
#include <qutim/statusactiongenerator.h>
#include <qutim/settingslayer.h>
#include "account/muc/jmucuser.h"
#include "account/muc/jmucsession.h"
#include "account/muc/jmucmanager.h"
#include "account/muc/jbookmarkmanager.h"
#include "account/roster/jmessagesession.h"
#include "account/muc/jconferenceconfig.h"
#include <QInputDialog>
#include <qutim/debug.h>
#include <jreen/logger.h>
#include <qutim/datasettingsobject.h>
#include <qutim/systemintegration.h>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <qutim/chatsession.h>
#include <QTextDocument>

namespace Jabber
{

enum JActionType
{
	SaveRemoveBookmarkAction,
	RoomConfigAction,
	RoomParticipantsAction,
	ChangeSubcriptionAction,
	KickAction,
	BanAction
};

class JProtocolPrivate
{
	Q_DECLARE_PUBLIC(JProtocol)
public:
	inline JProtocolPrivate(JProtocol *q) :
		accounts(new QHash<QString, JAccount *>),
		q_ptr(q)
	{
	}
	inline ~JProtocolPrivate()
	{
		delete accounts;
	}
	QHash<QString, JAccount *> *accounts;
	JProtocol *q_ptr;
	SettingsItem *mainSettings;
	QScopedPointer<ActionGenerator> subscribeGen;
	QScopedPointer<ActionGenerator> roomConfigGen;
	QScopedPointer<ActionGenerator> bookmarksGen;
	void checkSubscribe(JContact *c, QAction *a)
	{
		a->setEnabled(c->account()->status() != Status::Offline);
		LocalizedString str;
		switch(c->subscription()) {
		case Jreen::RosterItem::Both:
		case Jreen::RosterItem::To:
			str = QT_TRANSLATE_NOOP("Jabber", "Remove subscription");
			break;
		case Jreen::RosterItem::From:
		case Jreen::RosterItem::None:
		case Jreen::RosterItem::Invalid:
			str = QT_TRANSLATE_NOOP("Jabber", "Request subscription");
			break;
		default:
			break;
		}
		a->setText(str);
	}
	void checkRoomConfig(JMUCSession *s, QAction *a)
	{
		a->setEnabled(s->enabledConfiguring());
	}
	void checkBookMark(JMUCSession *s, QAction *a)
	{
		a->setEnabled(s->account()->status() != Status::Offline);
		if (!s->bookmark().isValid()) {
			a->setText(QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks"));
			a->setIcon(qutim_sdk_0_3::Icon("document-save"));
		} else {
			a->setText(QT_TRANSLATE_NOOP("Jabber", "Remove from bookmarks"));
			a->setIcon(qutim_sdk_0_3::Icon("edit-delete"));
		}
	}
	void _q_status_changed(qutim_sdk_0_3::Status)
	{
		QMap<QObject*, QAction*> actions = subscribeGen->actions();
		QMap<QObject*, QAction*>::const_iterator it = actions.constBegin();
		for(;it != actions.constEnd(); it++) {
			//TODO may be possible use reinterpret_cast?
			JContact *c = qobject_cast<JContact*>(it.key());
			Q_ASSERT(c);
			checkSubscribe(c, it.value());
		}
		//actions = roomConfigGen->actions();
		//it = actions.begin();
		//for(;it != actions.constEnd(); it++) {
		//	//TODO may be possible use reinterpret_cast?
		//	JMUCSession *s = qobject_cast<JMUCSession*>(it.key());
		//	Q_ASSERT(s);
		//	checkRoomConfig(s, it.value());
		//}
		actions = bookmarksGen->actions();
		it = actions.begin();
		for(;it != actions.constEnd(); it++) {
			//TODO may be possible use reinterpret_cast?
			JMUCSession *s = qobject_cast<JMUCSession*>(it.key());
			Q_ASSERT(s);
			checkBookMark(s, it.value());
		}
	}
	void _q_conference_bookmark_changed()
	{
		Q_Q(JProtocol);
		JMUCSession *s = qobject_cast<JMUCSession*>(q->sender());
		Q_ASSERT(s);
		foreach (QAction *a, bookmarksGen->actions(s))
			checkBookMark(s, a);
	}
	void _q_subscription_changed(Jreen::RosterItem::SubscriptionType)
	{
		Q_Q(JProtocol);
		JContact *c = qobject_cast<JContact*>(q->sender());
		Q_ASSERT(c);
		foreach (QAction *a, subscribeGen->actions(c))
			checkSubscribe(c,a);
	}
};

JProtocol *JProtocol::self = 0;

JProtocol::JProtocol() : d_ptr(new JProtocolPrivate(this))
{
	Q_ASSERT(!self);
	self = this;

	QDesktopServices::setUrlHandler("xmpp", this, "onUrlOpen");
}

JProtocol::~JProtocol()
{
	self = 0;
	QDesktopServices::unsetUrlHandler("xmpp");
}

QList<Account *> JProtocol::accounts() const
{
	QList<Account *> accounts;
	foreach (JAccount *account, d_ptr->accounts->values())
		accounts.append(account);
	return accounts;
}

Account *JProtocol::account(const QString &id) const
{
	return d_func()->accounts->value(id);
}

void JProtocol::loadActions()
{
	Q_D(JProtocol);
	d->mainSettings = new GeneralSettingsItem<JMainSettings>(Settings::Protocol,
															 Icon("im-jabber"),
															 QT_TRANSLATE_NOOP("Settings", "Main settings"));

	Settings::registerItem<JAccount>(d->mainSettings);

	Settings::registerItem<JMUCSession>(new GeneralDataSettingsItem<JConferenceConfig>(
	                                        Settings::Protocol,
	                                        QIcon(),
	                                        QT_TRANSLATE_NOOP("Settings", "Room configuration")));

	ActionGenerator *generator  = new ActionGenerator(Icon("im-kick-user"),
	                                                  QT_TRANSLATE_NOOP("Conference", "Kick"),
	                                                  this, SLOT(onKickUser(QObject*)));
	generator->addHandler(ActionVisibilityChangedHandler, this);
	generator->addProperty("actionType", KickAction);
	MenuController::addAction<JMUCUser>(generator);
	
	generator  = new ActionGenerator(Icon("im-ban-user"), QT_TRANSLATE_NOOP("Conference", "Ban"),
	                                 this, SLOT(onBanUser(QObject*)));
	generator->addHandler(ActionVisibilityChangedHandler, this);
	generator->addProperty("actionType", BanAction);
	MenuController::addAction<JMUCUser>(generator);
	
	//MenuController::addAction<JMessageSession>(
	//			new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Conference", "Convert to conference"),
	//								this, SLOT(onConvertToMuc(QObject*))));

	d->roomConfigGen.reset(new ActionGenerator(Icon("preferences-other"), QT_TRANSLATE_NOOP("Jabber", "Room's configuration"),
											   this, SLOT(onShowConfigDialog(QObject*))));
	d->roomConfigGen->addHandler(ActionCreatedHandler, this);
	d->roomConfigGen->setType(ActionTypeChatButton);
	MenuController::addAction<JMUCSession>(d->roomConfigGen.data());

	d->bookmarksGen.reset(new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks") ,
											  this, SLOT(onSaveRemoveBookmarks(QObject*))));
	d->bookmarksGen->addHandler(ActionCreatedHandler, this);
	d->bookmarksGen->setType(ActionTypeAdditional);
	d->bookmarksGen->setPriority(0);
	MenuController::addAction<JMUCSession>(d->bookmarksGen.data());

	d->subscribeGen.reset(new ActionGenerator(QIcon(), QT_TRANSLATE_NOOP("Jabber", "Subscription") ,
											  this, SLOT(onChangeSubscription(QObject*))));
	d->subscribeGen->addHandler(ActionCreatedHandler, this);
	d->subscribeGen->setType(0);
	d->subscribeGen->setPriority(0);
	MenuController::addAction<JContact>(d->subscribeGen.data());

	QList<Status> statuses;
	statuses << Status(Status::Online)
			 << Status(Status::FreeChat)
			 << Status(Status::Away)
			 << Status(Status::NA)
			 << Status(Status::DND)
			 << Status(Status::Invisible)
			 << Status(Status::Offline);
	Status connectingStatus(Status::Connecting);
	connectingStatus.initIcon("jabber");
	Status::remember(connectingStatus, "jabber");

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
	
	QInputDialog *dialog = new QInputDialog(QApplication::activeWindow());
	dialog->setWindowTitle(tr("Kick"));
    dialog->setLabelText(tr("Enter kick reason for %1").arg(user->name()));
	dialog->setTextValue(QString());
	dialog->setProperty("user", qVariantFromValue<QObject*>(user));
	SystemIntegration::open(dialog);
	connect(dialog, SIGNAL(textValueSelected(QString)), SLOT(onKickReasonSelected(QString)));
	connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
	connect(user, SIGNAL(destroyed()), dialog, SLOT(deleteLater()));
}

void JProtocol::onKickReasonSelected(const QString &reason)
{
	JMUCUser *user = static_cast<JMUCUser*>(sender()->property("user").value<QObject*>());
	JMUCSession *muc = static_cast<JMUCSession *>(user->upperUnit());
	muc->room()->kick(user->name(), reason);
}

void JProtocol::onBanUser(QObject *obj)
{
	JMUCUser *user = qobject_cast<JMUCUser*>(obj);
	Q_ASSERT(user);
	
	QInputDialog *dialog = new QInputDialog(QApplication::activeWindow());
	dialog->setWindowTitle(tr("Ban"));
    dialog->setLabelText(tr("Enter ban reason for %1").arg(user->name()));
	dialog->setTextValue(QString());
	dialog->setProperty("user", qVariantFromValue<QObject*>(user));
	SystemIntegration::open(dialog);
	connect(dialog, SIGNAL(textValueSelected(QString)), SLOT(onBanReasonSelected(QString)));
	connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
	connect(user, SIGNAL(destroyed()), dialog, SLOT(deleteLater()));
}

void JProtocol::onBanReasonSelected(const QString &reason)
{
	JMUCUser *user = static_cast<JMUCUser*>(sender()->property("user").value<QObject*>());
	JMUCSession *muc = static_cast<JMUCSession *>(user->upperUnit());
	muc->room()->ban(user->name(), reason);
}

void JProtocol::onConvertToMuc(QObject *obj)
{
	Q_UNUSED(obj);
	//JMessageSession *session = qobject_cast<JMessageSession*>(obj);
	//Q_ASSERT(session);
	//session->convertToMuc();
}

void JProtocol::onShowConfigDialog(QObject* obj)
{
	JMUCSession *room = qobject_cast<JMUCSession*>(obj);
	Q_ASSERT(room);
	if (!room->enabledConfiguring())
		return;
	SettingsLayer *layer = ServiceManager::getByName<SettingsLayer*>("SettingsLayer");
	layer->show(room);
	//	if (room->enabledConfiguring())
	//		room->showConfigDialog();
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
		room->setBookmark(Jreen::Bookmark::Conference());
	}
}

void JProtocol::onChangeSubscription(QObject *obj)
{
	JContact *contact = qobject_cast<JContact*>(obj);
	Q_ASSERT(contact);
	switch(contact->subscription()) {
	case Jreen::RosterItem::Both:
	case Jreen::RosterItem::To:
		contact->removeSubscription();
		break;
	case Jreen::RosterItem::From:
	case Jreen::RosterItem::None:
	case Jreen::RosterItem::Invalid:
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
		Jreen::JID jid = id;
		addAccount(new JAccount(jid.bare()), true);
	}
}

Account *JProtocol::doCreateAccount(const QString &id, const QVariantMap &parameters)
{
	JAccount *account = new JAccount(id);
	account->updateParameters(parameters);
//	QString password = properties.value(QLatin1String("password")).toString();
//	QString server = properties.value(QLatin1String("connect-server")).toString();
//	int port = properties.value(QLatin1String("port"), 5222).toInt();
//	QString resource = properties.value(QLatin1String("resource")).toString();
//	int priority = properties.value(QLatin1String("priority"), 30).toInt();
//	if (!password.isEmpty())
//		account->setPasswd(password);
	{
		Config config = Protocol::config();
		config.beginGroup(QLatin1String("general"));
		QStringList accounts = config.value(QLatin1String("accounts"), QStringList());
		accounts << account->id();
		config.setValue(QLatin1String("accounts"), accounts);
	}
//	{
//		Config config = account->config();
//		config.setValue(QLatin1String("resource"), resource);
//		config.setValue(QLatin1String("priority"), priority);
//		config.setValue(QLatin1String("server"), server);
//		config.setValue(QLatin1String("port"), port);
//	}
	addAccount(account, false);
	return account;
}

void JProtocol::addAccount(JAccount *account, bool loadSettings)
{
	Q_D(JProtocol);
	if (loadSettings)
		account->loadSettings();
	d->accounts->insert(account->id(), account);
	emit accountCreated(account);

	connect(account, SIGNAL(destroyed(QObject*)),
			this, SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(_q_status_changed(qutim_sdk_0_3::Status)));
	d->mainSettings->connect(SIGNAL(saved()),
							 account, SLOT(loadSettings()));
}

QVariant JProtocol::data(DataType type)
{
	switch (type) {
	case ProtocolIdName:
		return QLatin1String("Jabber ID");
	case ProtocolContainsContacts:
		return true;
	default:
		return QVariant();
	}
}

Jreen::Presence::Type JStatus::statusToPresence(const Status &status)
{
	Jreen::Presence::Type presence;
	switch (status.type()) {
	case Status::Offline:
		presence = Jreen::Presence::Unavailable;
		break;
	case Status::Online:
		presence = Jreen::Presence::Available;
		break;
	case Status::Away:
		presence = Jreen::Presence::Away;
		break;
	case Status::FreeChat:
		presence = Jreen::Presence::Chat;
		break;
	case Status::DND:
		presence = Jreen::Presence::DND;
		break;
	case Status::NA:
		presence = Jreen::Presence::XA;
		break;
	case Status::Invisible:
		presence = Jreen::Presence::XA;
		break;
	default:
		presence = Jreen::Presence::Invalid;
	}
	return presence;
}

Status JStatus::presenceToStatus(Jreen::Presence::Type presence)
{
	Status::Type status;
	switch (presence) {
	case Jreen::Presence::Available:
		status = Status::Online;
		break;
	case Jreen::Presence::Away:
		status = Status::Away;
		break;
	case Jreen::Presence::Chat:
		status = Status::FreeChat;
		break;
	case Jreen::Presence::DND:
		status = Status::DND;
		break;
	case Jreen::Presence::XA:
		status = Status::NA;
		break;
	case Jreen::Presence::Error:
	case Jreen::Presence::Unavailable:
	default: //TODO probe,subscribe etc. isn't offline status
		status = Status::Offline;
	}
	return Status::instance(status, "jabber");
}

bool JProtocol::event(QEvent *ev)
{
	Q_D(JProtocol);
	if (ev->type() == ActionCreatedEvent::eventType()) {
		ActionCreatedEvent *event = static_cast<ActionCreatedEvent*>(ev);
		QAction *action = event->action();
		QObject *controller = event->controller();
		if (event->generator() == d->subscribeGen.data()) {
			JContact *c = qobject_cast<JContact*>(controller);
			Q_ASSERT(c);
			d->checkSubscribe(c, action);
			connect(c, SIGNAL(subscriptionChanged(Jreen::RosterItem::SubscriptionType)),
			        this, SLOT(_q_subscription_changed(Jreen::RosterItem::SubscriptionType)));
		} else if (JMUCSession *s = qobject_cast<JMUCSession*>(controller)) {
			if(event->generator() == d->bookmarksGen.data()) {
				d->checkBookMark(s, action);
				connect(s, SIGNAL(bookmarkChanged(Jreen::Bookmark::Conference)),
				        this, SLOT(_q_conference_bookmark_changed()));
			}
		}
		return true;
	} else if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
		ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
		QAction *action = event->action();		
		JActionType type = static_cast<JActionType>(action->property("actionType").toInt());
		if (event->isVisible()) {
			switch (type) {
			case RoomConfigAction: {
				break;
			}
			case SaveRemoveBookmarkAction: {
				break;
			}
			case ChangeSubcriptionAction: {
				break;
			}
			case KickAction: {
				JMUCUser *user = qobject_cast<JMUCUser*>(event->controller());
				action->setVisible(user->muc()->room()->canKick(user->name()));
				break;
			}
			case BanAction: {
				JMUCUser *user = qobject_cast<JMUCUser*>(event->controller());
				action->setVisible(user->muc()->room()->canBan(user->name()));
				break;
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

void JProtocol::virtual_hook(int id, void *data)
{
	switch (id) {
//	case SupportedAccountParametersHook: {
//		QStringList &properties = *reinterpret_cast<QStringList*>(data);
//		properties << QLatin1String("connect-server")
//		           << QLatin1String("port")
//		           << QLatin1String("password")
//		           << QLatin1String("resource")
//		           << QLatin1String("priority")
//		           << QLatin1String("fallback-conference-server")
//		           << QLatin1String("bosh-host")
//		           << QLatin1String("bosh-port");
//		break;
//	}
//	case CreateAccountHook: {
//		CreateAccountArgument &argument = *reinterpret_cast<CreateAccountArgument*>(data);
//		argument.account = doCreateAccount(argument.id, argument.parameters);
//		break;
//	}
	default:
		Protocol::virtual_hook(id, data);
	}
}

void JProtocol::onAccountDestroyed(QObject *obj)
{
	JAccount *acc = reinterpret_cast<JAccount*>(obj);
	d_ptr->accounts->remove(d_ptr->accounts->key(acc));
}

void JProtocol::onUrlOpen(const QUrl &url)
{
	qDebug() << "Handle url: " << url;

	ServicePointer<QObject> joinGroupChatDlg = ServicePointer<QObject>("JoinGroupChat");
	if(!joinGroupChatDlg)
		return;

	QUrlQuery q;
	q.setQueryDelimiters('=', ';');
	q.setQuery(url.query());

	if(q.hasQueryItem("join"))
	{
		QMetaObject::invokeMethod(joinGroupChatDlg.data(), "onJoinGroupChatTriggered", Q_ARG(QString, url.path()));
		return;
	}

	qDebug() << q.queryItems();
	if(q.hasQueryItem("message") && q.hasQueryItem("body"))
		foreach (ChatSession *session, ChatLayer::instance()->sessions())
			if(session->isActive() && session->unit()->id().startsWith(url.path()))
				session->getInputField()->setPlainText(q.queryItemValue("body"));

	// TODO: "message" option

}
}

#include "moc_jprotocol.cpp"

