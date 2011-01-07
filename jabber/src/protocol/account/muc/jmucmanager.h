/****************************************************************************
 *  jmucmanager.h
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
#ifndef JMUCMANAGER_H
#define JMUCMANAGER_H

#include <QObject>
#include <qutim/chatunit.h>
#include <jreen/presence.h>
#include <qutim/dataforms.h>

namespace qutim_sdk_0_3 
{
class Conference;
}

namespace Jabber
{
using namespace qutim_sdk_0_3;
using namespace jreen;

class JAccount;
class JBookmarkManager;
struct JMUCManagerPrivate;
class JMUCSession;

class JMUCManager : public QObject
{
	Q_OBJECT
public:
	JMUCManager(JAccount *account, QObject *parent = 0);
	~JMUCManager();
	ChatUnit *muc(const jreen::JID &jid);
	JBookmarkManager *bookmarkManager();
	void syncBookmarks();
	void join(const QString &conference, const QString &nick = QString(), const QString &password = QString());
	void setPresenceToRooms(jreen::Presence::Type presence);
	void leave(const QString &room);
	bool event(QEvent *event);
	void appendMUCSession(JMUCSession *room);
signals:
	void conferenceCreated(qutim_sdk_0_3::Conference*);
private slots:
	void bookmarksChanged();
	void closeMUCSession();
protected:
	void createActions(JMUCSession *room);
	void closeMUCSession(JMUCSession *room);
private:
	QScopedPointer<JMUCManagerPrivate> p;
};
}

#endif // JMUCMANAGER_H
