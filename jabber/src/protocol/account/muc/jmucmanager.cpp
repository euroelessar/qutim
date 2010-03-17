#include "../jaccount.h"
#include "jmucmanager.h"
#include "jbookmarkmanager.h"
#include "jinvitemanager.h"
#include "jmucjoin.h"
#include "jmucsession.h"
#include <qutim/messagesession.h>

namespace Jabber
{
	struct JMUCManagerPrivate
	{
		JAccount *account;
		JBookmarkManager *bookmarkManager;
		JInviteManager *inviteManager;
		QHash<QString, JMUCSession *> rooms;
	};

	JMUCManager::JMUCManager(JAccount *account, QObject *parent) : QObject(parent), p(new JMUCManagerPrivate)
	{
		p->account = account;
		p->bookmarkManager = new JBookmarkManager(p->account);
		p->inviteManager = new JInviteManager(p->account);
	}

	JMUCManager::~JMUCManager()
	{

	}

	void JMUCManager::openJoinWindow(const QString &conference, const QString &nick, const QString &password)
	{
		JMUCJoin *joinConference = new JMUCJoin(p->account);
		joinConference->setConference(conference, nick, password);
		joinConference->show();
	}

	void JMUCManager::openJoinWindow()
	{
		openJoinWindow("", "", "");
	}

	void JMUCManager::join(const QString &conference, const QString &nick, const QString &password)
	{
		if (!p->rooms.contains(conference)) {
			JID jid = JID(conference.toStdString());
			jid.setResource(nick.toStdString());
			JMUCSession *room = new JMUCSession(jid, p->account);
			room->join();
			p->rooms.insert(conference, room);
			ChatLayer::get(room, true)->activate();
		}
	}

	ChatUnit *JMUCManager::muc(const QString &jid)
	{
		return p->rooms.value(jid);
	}

	JBookmarkManager *JMUCManager::bookmarkManager()
	{
		return p->bookmarkManager;
	}

	void JMUCManager::syncBookmarks()
	{
		p->bookmarkManager->sync();
	}
}
