#include "jaccount_p.h"
#include "roster/jroster.h"
#include "roster/jcontact.h"
#include "roster/jcontactresource.h"
#include "roster/jmessagehandler.h"
#include "connection/jserverdiscoinfo.h"
#include "servicediscovery/jservicebrowser.h"
#include "servicediscovery/jservicediscovery.h"
#include "../jprotocol.h"
#include "muc/jmucmanager.h"
#include "muc/jmucuser.h"
#include <qutim/systeminfo.h>
#include <qutim/passworddialog.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <qutim/dataforms.h>
#include <jreen/jid.h>

namespace Jabber {

class JPasswordValidator : public QValidator
{
	State validate(QString &input, int &pos) const
	{
		if (input.isEmpty())
			return Intermediate;
		else
			return Acceptable;
	}
};

void JAccountPrivate::setPresence(jreen::Presence presence)
{
	Q_Q(JAccount);
	Status now = q->status();
	now.setType(JStatus::presenceToStatus(presence.presence()));
	now.setText(presence.status());
	q->setStatus(now);
	emit q->statusChanged(now,q->status());
}

void JAccountPrivate::onConnected()
{
	Status s = q_func()->status();
	client.setPresence(JStatus::statusToPresence(s),
					   s.text());
}

void JAccountPrivate::onDisconnected()
{
	Q_Q(JAccount);
	Status now = q->status();
	now.setType(Status::Offline);
	emit q->statusChanged(now,q->status());
}

JAccount::JAccount(const QString &jid) :
	Account(jid, JProtocol::instance()),
	d_ptr(new JAccountPrivate(this))
{
	Q_D(JAccount);
	p = d; //for dead code
	Account::setStatus(Status::instance(Status::Offline, "jabber"));
	d->client.setJID(jreen::JID(id()));
	d->roster = new JRoster(this);
	loadSettings();

	connect(&d->client,SIGNAL(connected()),
			d,SLOT(onConnected()));
	connect(&d->client,SIGNAL(disconnected()),
			d,SLOT(onDisconnected()));
	connect(&d->client, SIGNAL(serverFeaturesReceived(QSet<QString>)),
			d->roster, SLOT(load()));

	//old code
//	d->discoManager = 0;
//	d->connection = new JConnection(this);
//	d->connectionListener = new JConnectionListener(this);
//	Q_UNUSED(new JServerDiscoInfo(this));
	//d->roster = new JRoster(this);
	//d->messageHandler = new JMessageHandler(this);
//	d->conferenceManager = new JMUCManager(this);
	//	connect(d->conferenceManager, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
	//			SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
	//	d->connection->initExtensions();
}

JAccount::~JAccount()
{
}

ChatUnit *JAccount::getUnitForSession(ChatUnit *unit)
{
	if (qobject_cast<JContactResource*>(unit) && !qobject_cast<JMUCUser*>(unit))
		unit = unit->upperUnit();
	return unit;
}

ChatUnit *JAccount::getUnit(const QString &unitId, bool create)
{
//	Q_D(JAccount);
//	ChatUnit *unit = 0;
//	if (!!(unit = d->conferenceManager->muc(unitId)))
//		return unit;
//	return d->roster->contact(unitId, create);
	return 0;
}

void JAccount::beginChangeStatus(Presence::PresenceType presence)
{
	//	Q_D(JAccount);
	//		d->connection->setConnectionPresence(presence);
	//	Status previous = status();
	//	if (previous.type() == Status::Offline && presence != Presence::Unavailable) {
	//		d->client.connectToServer();
	//		Status newStatus = previous;
	//		newStatus.setType(Status::Connecting);
	//		Account::setStatus(newStatus);
	//		emit statusChanged(newStatus, previous);
	//	}
}

void JAccount::endChangeStatus(Presence::PresenceType)
{
	//	Q_D(JAccount);
	//	Status previous = status();
	//	Status newStatus = JProtocol::presenceToStatus(presence);
	//	debug() << "status changed from" << int(previous.type()) << "to" << newStatus << newStatus.text();
	//	if (previous == Status::Connecting && newStatus != Status::Offline)
	//		d->conferenceManager->syncBookmarks();
	//	if (previous != Status::Offline && newStatus == Status::Offline)
	//		d->roster->setOffline();
	//	d->conferenceManager->setPresenceToRooms(presence);
	//	Account::setStatus(newStatus);
	//	emit statusChanged(newStatus, previous);
}

void JAccount::loadSettings()
{
	Q_D(JAccount);
	Config general = config();
	general.beginGroup("general");
	d->client.setPassword(general.value("passwd", QString(), Config::Crypted));
	d->client.setPort(general.value("port", 5222));
	d->keepStatus = general.value("keepstatus", true);
	d->nick = general.value("nick", id());
	d->status = static_cast<Presence::PresenceType>(general.value("prevstatus", 8));
	general.endGroup();
}

void JAccount::setPasswd(const QString &passwd)
{
	Q_D(JAccount);
	config().group("general").setValue("passwd",passwd, Config::Crypted);
	d->passwd = passwd;
	config().sync();
}

JServiceDiscovery *JAccount::discoManager()
{
	Q_D(JAccount);
	if (!d->discoManager)
		d->discoManager = new JServiceDiscovery(this);
	return d->discoManager;
}

QString JAccount::name() const
{
	return d_func()->nick;
}

void JAccount::setNick(const QString &nick)
{
	Q_D(JAccount);
	Config general = config("general");
	general.setValue("nick", nick);
	general.sync();
	QString previous = d->nick;
	d->nick = nick;
	emit nameChanged(nick, previous);
}

const QString &JAccount::password(bool *ok)
{
	Q_D(JAccount);
	if (ok)
		*ok = true;
	if (d->passwd.isEmpty()) {
		if (ok)
			*ok = false;
		PasswordDialog *dialog = PasswordDialog::request(this);
		JPasswordValidator *validator = new JPasswordValidator();
		dialog->setValidator(validator);
		if (dialog->exec() == PasswordDialog::Accepted) {
			if (ok)
				*ok = true;
			d->passwd = dialog->password();
			if (dialog->remember()) {
				config().group("general").setValue("passwd", d->passwd, Config::Crypted);
				config().sync();
			}
		}
		delete validator;
		delete dialog;
	}
	return d->passwd;
}

JConnection *JAccount::connection()
{
	return d_func()->connection;
}

JMessageHandler *JAccount::messageHandler()
{
	return d_func()->messageHandler;
}

jreen::Client *JAccount::client() const
{
	//it may be dangerous
	return const_cast<jreen::Client*>(&d_func()->client);
}

JMUCManager *JAccount::conferenceManager()
{
	return d_func()->conferenceManager;
}

void JAccount::setStatus(Status status)
{
	Q_D(JAccount);
	Status old = this->status();

	if(old.type() == Status::Offline && status.type() != Status::Offline) {
		d->client.connectToServer();
		status.setType(Status::Connecting);
		emit statusChanged(status,old);
	} else if(status.type() == Status::Offline) {
		d->client.disconnectFromServer();
	} else {
		d->client.setPresence(JStatus::statusToPresence(status),
							  status.text());
	}
	Account::setStatus(status);
}

QString JAccount::getAvatarPath()
{
	return QString("%1/avatars/%2")
			.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
			.arg(protocol()->id());
}

QVariantList JAccountPrivate::toVariant(const QList<JBookmark> &list)
{
	QVariantList items;
	foreach (const JBookmark &bookmark, list) {
		QVariantMap item;
		QString name = bookmark.name.isEmpty() ? bookmark.conference : bookmark.name;
		item.insert("name",name);
		QVariantMap data;
		data.insert(QT_TRANSLATE_NOOP("Jabber", "Conference"),bookmark.conference);
		data.insert(QT_TRANSLATE_NOOP("Jabber", "Nick"),bookmark.nick);
		if (bookmark.autojoin)
			data.insert(QT_TRANSLATE_NOOP("Jabber", "Autojoin"),(QT_TRANSLATE_NOOP("Jabber", "Yes")).toString());
		item.insert("fields",data);
		items.append(item);
	}
	return items;
}

bool JAccount::event(QEvent *ev)
{
	Q_D(JAccount);
	if (ev->type() == qutim_sdk_0_3::Event::eventType()) {
		qutim_sdk_0_3::Event *event = static_cast<qutim_sdk_0_3::Event*>(ev);
		const char *id = qutim_sdk_0_3::Event::getId(event->id);
		debug() << id;
		if (!qstrcmp(id,"groupchat-join")) {
			qutim_sdk_0_3::DataItem item = event->at<qutim_sdk_0_3::DataItem>(0);
			conferenceManager()->join(item);
			if (event->at<bool>(1)) {
				qutim_sdk_0_3::DataItem nickItem("name", QT_TRANSLATE_NOOP("Jabber", "Name"),event->at<QString>(2));
				item.addSubitem(nickItem);
				conferenceManager()->bookmarkManager()->saveBookmark(item);
			}
			return true;
		} else if (!qstrcmp(id,"groupchat-bookmark-list")) {
			JBookmarkManager *manager = conferenceManager()->bookmarkManager();
			event->args[0] = d->toVariant(manager->bookmarks());
			event->args[1] = d->toVariant(manager->recent());
			return true;
		} else if (!qstrcmp(id,"groupchat-bookmark-remove")) {
			QString name = event->at<QString>(0);
			JBookmarkManager *manager = conferenceManager()->bookmarkManager();
			manager->removeBookmark(manager->indexOfBookmark(name));
			return true;
		} else if (!qstrcmp(id,"groupchat-bookmark-save")) {
			qutim_sdk_0_3::DataItem item = event->at<qutim_sdk_0_3::DataItem>(0);
			JBookmarkManager *manager = conferenceManager()->bookmarkManager();
			QString oldName = event->at<QString>(1);
			event->args[2] = manager->saveBookmark(item,oldName);
			return true;
		} else if (!qstrcmp(id,"groupchat-fields")) {
			QString name = event->args[1].toString();
			bool isBookmark = event->args[2].toBool();
			JBookmarkManager *manager = conferenceManager()->bookmarkManager();
			JBookmark bookmark = manager->find(name);
			if (bookmark.isEmpty())
				bookmark = manager->find(name,true);
			QVariant data = bookmark.isEmpty() ? QVariant() : qVariantFromValue(bookmark);
			event->args[0] = qVariantFromValue(conferenceManager()->fields(data,isBookmark));
			return true;
		}
	}
	return Account::event(ev);
}

QSet<QString> JAccount::features() const
{
	return d_func()->client.serverFeatures();
}

bool JAccount::checkFeature(const QString &feature) const
{
	return d_func()->client.serverFeatures().contains(feature);
}

bool JAccount::checkFeature(const std::string &feature) const
{
	return d_func()->client.serverFeatures().contains(QString::fromStdString(feature));
}

bool JAccount::checkIdentity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	Identities::const_iterator catItr = d->identities.find(category);
	return catItr == d->identities.constEnd() ? false : catItr->contains(type);
}

bool JAccount::checkIdentity(const std::string &category, const std::string &type) const
{
	return checkIdentity(QString::fromStdString(category), QString::fromStdString(type));
}

QString JAccount::identity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	Identities::const_iterator catItr = d->identities.find(category);
	return catItr == d->identities.constEnd() ? QString() : catItr->value(type);
}

std::string JAccount::identity(const std::string &category, const std::string &type) const
{
	return identity(QString::fromStdString(category), QString::fromStdString(type)).toStdString();
}

} // Jabber namespace
