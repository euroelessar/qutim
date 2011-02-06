/****************************************************************************
 *  jmucsession.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef JMUCSESSION_H
#define JMUCSESSION_H

#include <QByteArray>
#include <jreen/mucroom.h>
#include <jreen/jid.h>
#include <QHash>
#include <qutim/conference.h>
#include <jreen/bookmark.h>

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JMessageSession;
class JAccount;
class JMUCUser;
class JMUCSessionPrivate;
class JSessionConvertor;

class JMUCSession : public qutim_sdk_0_3::Conference
		//			,
		//			public gloox::MUCRoomHandler,
		//			public gloox::MUCRoomConfigHandler
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JMUCSession)
public:
	JMUCSession(const jreen::JID &room, const QString &password, JAccount *account);
	//			JMUCSession(JAccount *account, gloox::MUCRoom *room, const std::string &thread);
	~JMUCSession();
	QString id() const;
	bool sendMessage(const qutim_sdk_0_3::Message &message);
	bool sendPrivateMessage(const qutim_sdk_0_3::Message &message);
	void setBookmark(const jreen::Bookmark::Conference &bookmark);
	jreen::Bookmark::Conference bookmark();
	bool enabledConfiguring();
	bool isJoined();
	bool isAutoJoin();
	void setAutoJoin(bool join);
	bool isError();
	jreen::MUCRoom *room();
	qutim_sdk_0_3::Buddy *me() const;
	ChatUnit *participant(const QString &nick);
	QString title() const;
	void clearSinceDate();
	ChatUnitList lowerUnits();
	QString topic() const;
	void setTopic(const QString &topic);
	void setConferenceTopic(const QString &topic);
	void invite(qutim_sdk_0_3::Contact *contact, const QString &reason = QString());
protected:
	void loadSettings();
	//			// MUCRoomHandler
protected slots:
	void onParticipantPresence(const jreen::Presence &presence, const jreen::MUCRoom::Participant *part);
	void onMessage(const jreen::Message &msg, bool priv);
	void onSubjectChanged(const QString &subject, const QString &nick);
	void onServiceMessage(const jreen::Message &msg);
	void onError(jreen::Error::Ptr error);
	//			bool handleMUCRoomCreation(gloox::MUCRoom *room);
	//			void handleMUCInviteDecline(gloox::MUCRoom *room, const gloox::JID &invitee,
	//										const std::string &reason);
	//			void handleMUCError(gloox::MUCRoom *room, gloox::StanzaError error);
	//			void handleMUCInfo(gloox::MUCRoom *room, int features, const std::string &name,
	//								const gloox::DataForm *infoForm);
	//			void handleMUCItems(gloox::MUCRoom *room, const gloox::Disco::ItemList &items);
	//			// MUCRoomConfigHandler
	//			void handleMUCConfigList(gloox::MUCRoom *room, const gloox::MUCListItemList &items,
	//									 gloox::MUCOperation operation);
	//			void handleMUCConfigForm(gloox::MUCRoom *room, const gloox::DataForm &form);
	//			void handleMUCConfigResult(gloox::MUCRoom *room, bool success, gloox::MUCOperation operation);
	//			void handleMUCRequest(gloox::MUCRoom *room, const gloox::DataForm &form);
public slots:
	void join();
	void leave();
	void showConfigDialog();
private slots:
	void closeConfigDialog();
signals:
	void initClose();
	void bookmarkChanged(const jreen::Bookmark::Conference &bookmark);
private:
	QScopedPointer<JMUCSessionPrivate> d_ptr;
	friend class JSessionConvertor;
};
}

#endif // JMUCSESSION_H
