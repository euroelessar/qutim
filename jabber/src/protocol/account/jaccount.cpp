#include "jaccount.h"
#include "roster/jroster.h"
#include "roster/jcontact.h"
#include "roster/jcontactresource.h"
#include "roster/jmessagehandler.h"
#include "servicediscovery/jservicebrowser.h"
#include "servicediscovery/jservicediscovery.h"
#include "../jprotocol.h"
#include "muc/jmucmanager.h"
#include "muc/jbookmarkmanager.h"
#include "muc/jmucuser.h"
#include <qutim/systeminfo.h>
#include <qutim/passworddialog.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <qutim/dataforms.h>

namespace Jabber {

	struct JAccountPrivate
	{
		inline JAccountPrivate() : keepStatus(false), autoConnect(false) {}
		inline ~JAccountPrivate() {}
		JConnection *connection;
		JRoster *roster;
		JConnectionListener *connectionListener;
		JMessageHandler *messageHandler;
		QVariantList toVariant(const QList<JBookmark> &list);
		QString passwd;
		QString nick;
		bool keepStatus;
		bool autoConnect;
		Presence::PresenceType status;
		JMUCManager *conferenceManager;
		QPointer<JServiceDiscovery> discoManager;
	};

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

	JAccount::JAccount(const QString &jid) : Account(jid, JProtocol::instance()), p(new JAccountPrivate)
	{
		loadSettings();
		Account::setStatus(Status::instance(Status::Offline, "jabber"));
		p->discoManager = 0;
		p->connection = new JConnection(this);
		p->connectionListener = new JConnectionListener(this);
		p->roster = new JRoster(this);
		p->messageHandler = new JMessageHandler(this);
		p->conferenceManager = new JMUCManager(this);
		connect(p->conferenceManager, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)), 
				SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
		p->connection->initExtensions();
		autoconnect();
	}

	JAccount::~JAccount()
	{
	}

	ChatUnit *JAccount::getUnitForSession(ChatUnit *unit)
	{
		ChatUnit *upper = unit->upperUnit();
		if (qobject_cast<JContactResource*>(upper) && !qobject_cast<JMUCUser*>(upper))
			unit = upper->upperUnit();
		return p->messageHandler->getSession(unit);
	}

	ChatUnit *JAccount::getUnit(const QString &unitId, bool create)
	{
		ChatUnit *unit = 0;
		if (!!(unit = p->conferenceManager->muc(unitId)))
			return unit;
		if (!!(unit = p->messageHandler->getSession(unitId)))
			return unit;
		return p->roster->contact(unitId, create);
	}

	void JAccount::beginChangeStatus(Presence::PresenceType presence)
	{
		p->connection->setConnectionPresence(presence);				
		Status origin = status();
		if (origin.type() == Status::Offline) {
			origin.setType(Status::Connecting);
			Account::setStatus(origin);
			emit statusChanged(origin);
		}
	}

	void JAccount::endChangeStatus(Presence::PresenceType presence)
	{
		Status newStatus = JProtocol::presenceToStatus(presence);
		debug() << "new status" << newStatus << newStatus.text();
		if (status() == Status::Offline && newStatus != Status::Offline)
			p->conferenceManager->syncBookmarks();
		if (status() != Status::Offline && newStatus == Status::Offline)
			p->roster->setOffline();
		p->conferenceManager->setPresenceToRooms(presence);
		Account::setStatus(newStatus);
		emit statusChanged(newStatus);
	}

	void JAccount::autoconnect()
	{
		if (p->autoConnect) {
			if (p->keepStatus)
				beginChangeStatus(p->status);
			else
				beginChangeStatus(Presence::Available);
		}
	}

	void JAccount::loadSettings()
	{
		Config general = config();
		general.beginGroup("general");
		p->passwd = general.value("passwd", QString(), Config::Crypted);
		p->autoConnect = general.value("autoconnect", false);
		p->keepStatus = general.value("keepstatus", true);
		p->nick = general.value("nick", id());
		p->status = static_cast<Presence::PresenceType>(general.value("prevstatus", 8));
		general.endGroup();
	}

	JServiceDiscovery *JAccount::discoManager()
	{
		if (!p->discoManager)
			p->discoManager = new JServiceDiscovery(this);
		return p->discoManager;
	}

	QString JAccount::name() const
	{
		return p->nick;
	}

	void JAccount::setNick(const QString &nick)
	{
		Config general = config("general");
		general.setValue("nick", nick);
		general.sync();
		p->nick = nick;
		emit nameChanged(nick);
	}

	const QString &JAccount::password(bool *ok)
	{
		if (ok)
			*ok = true;
		if (p->passwd.isEmpty()) {
			if (ok)
				*ok = false;
			PasswordDialog *dialog = PasswordDialog::request(this);
			JPasswordValidator *validator = new JPasswordValidator();
			dialog->setValidator(validator);
			if (dialog->exec() == PasswordDialog::Accepted) {
				if (ok)
					*ok = true;
				p->passwd = dialog->password();
				if (dialog->remember()) {
					config().group("general").setValue("passwd", p->passwd, Config::Crypted);
					config().sync();
				}
			}
			delete validator;
			delete dialog;
		}
		return p->passwd;
	}

	JConnection *JAccount::connection()
	{
		return p->connection;
	}

	JMessageHandler *JAccount::messageHandler()
	{
		return p->messageHandler;
	}

	gloox::Client *JAccount::client()
	{
		return p->connection->client();
	}

	JMUCManager *JAccount::conferenceManager()
	{
		return p->conferenceManager;
	}

	void JAccount::setStatus(Status status)
	{
		beginChangeStatus(JProtocol::statusToPresence(status));
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
				event->args[0] = p->toVariant(manager->bookmarks());
				event->args[1] = p->toVariant(manager->recent());
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
} // Jabber namespace
