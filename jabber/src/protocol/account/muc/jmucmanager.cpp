#include <QClipboard>
#include <QMessageBox>
#include <QDebug>
#include "../jaccount.h"
#include "jmucmanager.h"
#include "jbookmarkmanager.h"
#include "jinvitemanager.h"
#include "jmucsession.h"
#include <qutim/messagesession.h>
#include <qutim/icon.h>
#include "../roster/jsoftwaredetection.h"
//jreen
#include <jreen/client.h>

namespace Jabber
{
	struct JMUCManagerPrivate
	{
		JAccount *account;
		JBookmarkManager *bookmarkManager;
//		JInviteManager *inviteManager;
		QHash<QString, JMUCSession *> rooms;
	};

	JMUCManager::JMUCManager(JAccount *account, QObject *parent) : QObject(parent), p(new JMUCManagerPrivate)
	{
		p->account = account;
		p->bookmarkManager = new JBookmarkManager(p->account);
//		p->inviteManager = new JInviteManager(p->account);
		connect(p->bookmarkManager, SIGNAL(bookmarksChanged()), SLOT(bookmarksChanged()));
	}

	JMUCManager::~JMUCManager()
	{
	}

	void JMUCManager::bookmarksChanged()
	{
		foreach (const QString &conference, p->rooms.keys()) {
			JMUCSession *muc = p->rooms.value(conference);
			Bookmark::Conference room = p->bookmarkManager->find(conference);
			muc->setBookmark(room);
			if (!room.isValid()) {
				if (!ChatLayer::instance()->getSession(muc, false))
					closeMUCSession(muc);
			}
		}
	}

	void JMUCManager::join(const QString &conference, const QString &nick, const QString &password)
	{
		JMUCSession *room = p->rooms.value(conference, 0);
		if (room && room->isError()) {
			room->setBookmark(Bookmark::Conference());
			closeMUCSession(room);
			room = 0;
			if (nick.isEmpty())
				return;
		}
		if (room && room->me() && !nick.isEmpty() && room->me()->name() != nick) {
			if (room->isJoined()) {
				QMessageBox::warning(0, tr("Join groupchat on")+" "+room->id(),
									 tr("You already in conference with another nick"));
			} else {
				room->setBookmark(Bookmark::Conference());
				closeMUCSession(room);
				room = 0;
			}
		}
		if (!room) {
			Q_ASSERT(!nick.isEmpty()); // Room doesn't exist. Nickname is required.
			jreen::JID jid = conference;
			jid.setResource(nick);
			room = new JMUCSession(jid, password, p->account);
			createActions(room);
			const QList<Bookmark::Conference> bookmarks = p->bookmarkManager->bookmarksList();
			for (int i = 0; i < bookmarks.count(); i++) {
				if (bookmarks[i].jid() == conference && bookmarks[i].nick() == nick) {
					room->setBookmark(bookmarks[i]);
					break;
				}
			}
			p->rooms.insert(conference, room);
			emit conferenceCreated(room);
		} else {
			room = p->rooms.value(conference);
		}
		room->join();
//		p->account->client()->registerPresenceHandler(JID(conference.toStdString()),
//													  p->account->connection()->softwareDetection());
		ChatSession *session = ChatLayer::get(room, true);
		connect(session, SIGNAL(destroyed()), room, SIGNAL(initClose()));
		connect(room, SIGNAL(initClose()), SLOT(closeMUCSession()));
		session->activate();

		bookmarkManager()->saveRecent(conference,nick,password);
	}

	void JMUCManager::closeMUCSession()
	{
		JMUCSession *room = qobject_cast<JMUCSession *>(sender());
		closeMUCSession(room);
	}

	void JMUCManager::closeMUCSession(JMUCSession *room)
	{
//		if (room && !room->isJoined()) {
//			room->clearSinceDate();
//			p->account->client()->removePresenceHandler(room->id().toStdString(),
//														p->account->connection()->softwareDetection());
//			if (room->bookmarkIndex() == -1) {
//				p->rooms.remove(room->id());
//				room->deleteLater();
//				//TODO: remove conference from roster
//			}
//		}
	}
	
	void JMUCManager::appendMUCSession(JMUCSession *room)
	{
		Q_ASSERT(room);
		p->rooms.insert(room->id(), room);
	}

	void JMUCManager::setPresenceToRooms(jreen::Presence::Type presence)
	{
		if (presence == jreen::Presence::Unavailable) {
			foreach (JMUCSession *room, p->rooms) {
				if(room->isJoined()) {
					room->setAutoJoin(true);
					room->leave();
				}
			}
		} else {
			foreach (JMUCSession *room, p->rooms)
				if(room->isJoined() || room->isAutoJoin())
					room->join();
		}
	}

	ChatUnit *JMUCManager::muc(const jreen::JID &jid)
	{
		if (JMUCSession *muc = p->rooms.value(jid.bare())) {
			if (jid.isBare())
				return muc;
			else
				return muc->participant(jid.resource());
		}
		return 0;
	}

	JBookmarkManager *JMUCManager::bookmarkManager()
	{
		return p->bookmarkManager;
	}

	void JMUCManager::syncBookmarks()
	{
		p->bookmarkManager->sync();
	}

	void JMUCManager::createActions(JMUCSession *room)
	{
		//TODO, Rewrite! Only one action generator for protocol is needed!
	}

	bool JMUCManager::event(QEvent *event)
	{
		return QObject::event(event);
	}

	void JMUCManager::leave(const QString &room)
	{
		JMUCSession *muc = p->rooms.value(room);
		muc->leave();
	}
}
