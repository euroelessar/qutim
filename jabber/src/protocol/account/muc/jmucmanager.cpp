#include <QClipboard>
#include "../jaccount.h"
#include "jmucmanager.h"
#include "jbookmarkmanager.h"
#include "jinvitemanager.h"
#include "jmucjoin.h"
#include "jmucsession.h"
#include <qutim/messagesession.h>
#include <qutim/contactlist.h>
#include <qutim/icon.h>

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
			JBookmark room("", conference, p->rooms.value(conference)->me()->name(), "");
			if (p->bookmarkManager->bookmarks().contains(room)) {
				int num = p->bookmarkManager->bookmarks().indexOf(room);
				p->rooms.value(conference)->setBookmarkIndex(num);
			} else {
				p->rooms.value(conference)->setBookmarkIndex(-1);
			}
		}
	}

	void JMUCManager::openJoinWindow(const QString &conference, const QString &nick, const QString &password,
			const QString &name)
	{
		JMUCJoin *joinConference = new JMUCJoin(p->account);
		joinConference->setConference(conference, nick, password, name);
		joinConference->show();
	}

	void JMUCManager::openJoinWindow()
	{
		openJoinWindow("", "", "");
	}

	void JMUCManager::join(const QString &conference, const QString &nick, const QString &password)
	{
		JMUCSession *room;
		if (!p->rooms.contains(conference)) {
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
		} else {
			room = p->rooms.value(conference);
		}
		room->join();
		ChatLayer::get(room, true)->activate();
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
		if (presence == Presence::Unavailable)
			foreach (JMUCSession *room, p->rooms)
				room->leave();
		else
			foreach (JMUCSession *room, p->rooms)
				room->join();
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
		p->actions.insert(generator, JoinAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Leave from conference"),
				this, SLOT(leave()));
		generator->addCreationHandler(this);
		generator->setType(0);
		p->actions.insert(generator, LeaveAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Copy room address to clipboard"),
				this, SLOT(copyJIDToClipboard()));
		generator->addCreationHandler(this);
		generator->setType(1);
		p->actions.insert(generator, CopyJIDAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Save to bookmarks"),
				this, SLOT(saveToBookmarks()));
		generator->addCreationHandler(this);
		generator->setType(2);
		p->actions.insert(generator, SaveToBookmarkAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Remove from bookmarks"),
				this, SLOT(removeFromBookmarks()));
		generator->addCreationHandler(this);
		generator->setType(2);
		p->actions.insert(generator, RemoveFromBookmarkAction);
		room->addAction(generator);
		generator = new ActionGenerator(Icon(""), QT_TRANSLATE_NOOP("Jabber", "Room's configuration"),
				room, SLOT(showConfigDialog()));
		generator->addCreationHandler(this);
		generator->setType(3);
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
			case LeaveAction:
			case CopyJIDAction:
				break;
			case SaveToBookmarkAction:
				if (room->bookmarkIndex() == -1)
					action->setVisible(true);
				else
					action->setVisible(false);
				break;
			case RemoveFromBookmarkAction:
				if (room->bookmarkIndex() == -1)
					action->setVisible(false);
				else
					action->setVisible(true);
				break;
			case RoomConfigAction:
				action->setVisible(room->enabledConfiguring());
				break;
			case RoomParticipantsAction:
				action->setVisible(false);
				break;
			}
			return true;
		}
		return false;
	}
}
