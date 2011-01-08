/****************************************************************************
 *  jmucmanager.cpp
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/
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
#include <qutim/status.h>
#include "../roster/jsoftwaredetection.h"
//jreen
#include <jreen/client.h>
#include <jreen/privacymanager.h>

namespace Jabber
{
struct JMUCManagerPrivate
{
	JAccount *account;
	JBookmarkManager *bookmarkManager;
	//JInviteManager *inviteManager;
	QHash<QString, JMUCSession *> rooms;
	QList<JMUCSession *> roomsToConnect;
	void connectAll()
	{
		foreach (JMUCSession *session, rooms) {
			jreen::MUCRoom *room = session->room();
			if (!room->isJoined() && room->presence() != Presence::Unavailable
				&& !roomsToConnect.contains(session)) {
				session->join();
			}
		}
		for (int i = 0; i < roomsToConnect.size(); i++)
			roomsToConnect.at(i)->join();
		roomsToConnect.clear();
	}
};

JMUCManager::JMUCManager(JAccount *account, QObject *parent) : QObject(parent), p(new JMUCManagerPrivate)
{
	p->account = account;
	p->bookmarkManager = new JBookmarkManager(p->account);
	//p->inviteManager = new JInviteManager(p->account);
	connect(p->bookmarkManager, SIGNAL(bookmarksChanged()), SLOT(bookmarksChanged()));
	connect(p->account->privacyManager(), SIGNAL(listReceived(QString,QList<jreen::PrivacyItem>)),
			this, SLOT(onListReceived(QString,QList<jreen::PrivacyItem>)));
	connect(p->account->privacyManager(), SIGNAL(activeListChanged(QString)),
			this, SLOT(onActiveListChanged(QString)));
}

JMUCManager::~JMUCManager()
{
}

void JMUCManager::onListReceived(const QString &name, const QList<jreen::PrivacyItem> &items)
{
	jreen::PrivacyManager *manager = p->account->privacyManager();
	qDebug() << Q_FUNC_INFO << name << manager->activeList();
	if (name == manager->activeList()) {
		QSet<QString> badList;
		QSet<jreen::JID> conferences;
		foreach (JMUCSession *muc, p->rooms)
			conferences << muc->room()->id();
		QMutableSetIterator<jreen::JID> it(conferences);
		for (int i = 0; i < items.size() && !conferences.isEmpty(); i++) {
			it.toFront();
			while (it.hasNext()) {
				const JID &jid = it.next();
				const PrivacyItem &item = items.at(i);
				qDebug() << jid << item.type() << item.jid() << item.check(jid) << item.action() << item.stanzaTypes();
				if ((item.stanzaTypes() & PrivacyItem::PresenceOut) && item.check(jid)) {
					if (item.action() == PrivacyItem::Deny)
						badList << jid.domain();
					conferences.remove(jid);
				}
			}
		}
		if (badList.isEmpty()) {
			p->connectAll();
			return;
		}
		QSetIterator<QString> it2(badList);
		QList<jreen::PrivacyItem> newList = items;
		while (it2.hasNext()) {
			QString domain = it2.next();
			jreen::JID jid;
			jid.setDomain(domain);
			jreen::PrivacyItem item;
			item.setAction(PrivacyItem::Allow);
			item.setOrder(0);
			item.setJID(jid);
			item.setStanzaTypes(PrivacyItem::PresenceOut);
			newList.prepend(item);
		}
		manager->setList(name, newList);
		p->connectAll();
	}
}

void JMUCManager::onActiveListChanged(const QString &name)
{
	p->account->privacyManager()->requestList(name);
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
		jreen::PrivacyManager *manager = p->account->privacyManager();
		emit conferenceCreated(room);
		p->roomsToConnect << room;
		manager->requestList(manager->activeList());
	} else {
		room = p->rooms.value(conference);
		room->join();
	}
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
