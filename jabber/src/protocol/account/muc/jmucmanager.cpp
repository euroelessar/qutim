#include <QClipboard>
#include <QMessageBox>
#include <QDebug>
#include "../jaccount.h"
#include "jmucmanager.h"
#include "jbookmarkmanager.h"
#include "jinvitemanager.h"
#include "jmucjoin.h"
#include "jmucsession.h"
#include <qutim/messagesession.h>
#include <qutim/icon.h>
#include "../roster/jsoftwaredetection.h"

namespace Jabber
{
	enum JMUCActionType
	{
		JoinAction,
		LeaveAction,
		CopyJIDAction,
		SaveToBookmarkAction,
		RemoveFromBookmarkAction,
		RoomConfigAction,
		RoomParticipantsAction
	};

	struct JMUCManagerPrivate
	{
		JAccount *account;
		JBookmarkManager *bookmarkManager;
		JInviteManager *inviteManager;
		QHash<QString, JMUCSession *> rooms;
		QMap<ActionGenerator*, JMUCActionType> actions;
	};

	JMUCManager::JMUCManager(JAccount *account, QObject *parent) : QObject(parent), p(new JMUCManagerPrivate)
	{
		p->account = account;
		p->bookmarkManager = new JBookmarkManager(p->account);
		p->inviteManager = new JInviteManager(p->account);
		connect(p->bookmarkManager, SIGNAL(bookmarksChanged()), SLOT(bookmarksChanged()));
	}

	JMUCManager::~JMUCManager()
	{

	}

	void JMUCManager::bookmarksChanged()
	{
		foreach (QString conference, p->rooms.keys())
		{
			JMUCSession *muc = p->rooms.value(conference);
			JBookmark room(QString(), conference,
						   (muc && muc->me()) ? muc->me()->name() : QString(),
						   QString());
			if (p->bookmarkManager->bookmarks().contains(room)) {
				int num = p->bookmarkManager->bookmarks().indexOf(room);
				muc->setBookmarkIndex(num);
			} else {
				muc->setBookmarkIndex(-1);
				if (!ChatLayer::instance()->getSession(muc, false))
					closeMUCSession(muc);
			}
		}
	}

	void JMUCManager::openJoinWindow(const QString &conference, const QString &nick, const QString &password,
			const QString &name)
	{
		JMUCJoin *joinConference = new JMUCJoin(p->account);
		joinConference->setConference(conference, nick, password, name);
#if defined (Q_OS_SYMBIAN)
		joinConference->showMaximized();
#else
		joinConference->show();
#endif
	}

	void JMUCManager::openJoinWindow()
	{
		openJoinWindow(QString(), QString(), QString());
	}

	void JMUCManager::join(const QString &conference, const QString &nick, const QString &password)
	{
		JMUCSession *room = p->rooms.value(conference, 0);
		if (room && room->isError()) {
			room->setBookmarkIndex(-1);
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
				room->setBookmarkIndex(-1);
				closeMUCSession(room);
				room = 0;
			}
		}
		if (!room) {
			Q_ASSERT(!nick.isEmpty()); // Room doesn't exist. Nickname is required.
			JID jid = JID(conference.toStdString());
			jid.setResource(nick.toStdString());
			room = new JMUCSession(jid, password, p->account);
			createActions(room);
			int count = p->bookmarkManager->bookmarks().count();
			room->setBookmarkIndex(-1);
			for (int num = 0; num < count; num++)
				if (p->bookmarkManager->bookmarks()[num].conference == conference
						&& p->bookmarkManager->bookmarks()[num].nick == nick) {
					room->setBookmarkIndex(num);
					break;
				}
			p->rooms.insert(conference, room);
			emit conferenceCreated(room);
		} else {
			room = p->rooms.value(conference);
		}
		room->join();
		p->account->client()->registerPresenceHandler(JID(conference.toStdString()),
													  p->account->connection()->softwareDetection());
		ChatSession *session = ChatLayer::get(room, true);
		connect(session, SIGNAL(destroyed()), room, SIGNAL(initClose()));
		connect(room, SIGNAL(initClose()), SLOT(closeMUCSession()));
		session->activate();
	}

	void JMUCManager::closeMUCSession()
	{
		JMUCSession *room = qobject_cast<JMUCSession *>(sender());
		closeMUCSession(room);
	}

	void JMUCManager::closeMUCSession(JMUCSession *room)
	{
		if (room && !room->isJoined()) {
			room->clearSinceDate();
			p->account->client()->removePresenceHandler(room->id().toStdString(), 
														p->account->connection()->softwareDetection());
			if (room->bookmarkIndex() == -1) {
				p->rooms.remove(room->id());
				room->deleteLater();
				//TODO: remove conference from roster
			}
		}
	}
	
	void JMUCManager::appendMUCSession(JMUCSession *room)
	{
		Q_ASSERT(room);
		p->rooms.insert(room->id(), room);
	}

	void JMUCManager::join()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		JMUCSession *room = MenuController::getController<JMUCSession>(action);
		if (room)
			join(room->id());
	}

	void JMUCManager::setPresenceToRooms(Presence::PresenceType presence)
	{
		if (presence == Presence::Unavailable) {
			foreach (JMUCSession *room, p->rooms)
				if(room->isJoined()) {
					room->setAutoJoin(true);
					room->leave();
				}
		} else {
			foreach (JMUCSession *room, p->rooms)
				if(room->isJoined() || room->isAutoJoin())
					room->join();
		}
	}

	void JMUCManager::leave(const QString &room)
	{
		JMUCSession *muc = p->rooms.value(room);
		muc->leave();
	}

	void JMUCManager::leave()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		JMUCSession *room = MenuController::getController<JMUCSession>(action);
		leave(room->id());
	}

	ChatUnit *JMUCManager::muc(const QString &jid)
	{
		QString room(jid.section('/', 0, 0));
		QString user(jid.section('/', 1));
		if (JMUCSession *muc = p->rooms.value(room)) {
			if (user.isEmpty())
				return muc;
			else
				return muc->participant(user);
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

	void JMUCManager::saveToBookmarks()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		JMUCSession *room = MenuController::getController<JMUCSession>(action);
		openJoinWindow(room->id(), room->me()->name(), "", room->id());
	}

	void JMUCManager::removeFromBookmarks()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		JMUCSession *room = MenuController::getController<JMUCSession>(action);
		p->bookmarkManager->removeBookmark(room->bookmarkIndex());
	}

	void JMUCManager::copyJIDToClipboard()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		JMUCSession *room = MenuController::getController<JMUCSession>(action);
		QApplication::clipboard()->setText(room->id());
	}

	void JMUCManager::createActions(JMUCSession *room)
	{
		ActionGenerator *generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Join to conference"),
				this, SLOT(join()));
		generator->addCreationHandler(this);
		generator->setType(0);
		generator->setPriority(4);
		p->actions.insert(generator, JoinAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Leave from conference"),
				this, SLOT(leave()));
		generator->addCreationHandler(this);
		generator->setType(0);
		generator->setPriority(3);
		p->actions.insert(generator, LeaveAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Copy room address to clipboard"),
				this, SLOT(copyJIDToClipboard()));
		generator->addCreationHandler(this);
		generator->setType(0);
		generator->setPriority(2);
		p->actions.insert(generator, CopyJIDAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks"),
				this, SLOT(saveToBookmarks()));
		generator->addCreationHandler(this);
		generator->setType(0);
		generator->setPriority(1);
		p->actions.insert(generator, SaveToBookmarkAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Remove from bookmarks"),
				this, SLOT(removeFromBookmarks()));
		generator->addCreationHandler(this);
		generator->setType(0);
		generator->setPriority(0);
		p->actions.insert(generator, RemoveFromBookmarkAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Room's configuration"),
				room, SLOT(showConfigDialog()));
		generator->addCreationHandler(this);
		generator->setType(1);
		p->actions.insert(generator, RoomConfigAction);
		room->addAction(generator);
	}

	bool JMUCManager::event(QEvent *event)
	{
		if (event->type() == ActionCreatedEvent::eventType()) {
			ActionCreatedEvent *actionEvent = static_cast<ActionCreatedEvent*>(event);
			ActionGenerator *generator = actionEvent->generator();
			QAction *action = actionEvent->action();
			JMUCSession *room = MenuController::getController<JMUCSession>(action);
			switch (p->actions.value(generator)) {
			case JoinAction:
				action->setVisible(!room->isJoined());
				break;
			case LeaveAction:
				action->setVisible(room->isJoined());
				break;
			case CopyJIDAction:
				break;
			case SaveToBookmarkAction:
				action->setVisible(room->bookmarkIndex() == -1 ? true : false);
				break;
			case RemoveFromBookmarkAction:
				action->setVisible(room->bookmarkIndex() == -1 ? false : true);
				break;
			case RoomConfigAction:
				action->setVisible(room->enabledConfiguring());
				break;
			case RoomParticipantsAction:
				action->setVisible(false);
			default:
				break;
			}
			return true;
		}
		return QObject::event(event);
	}
}
