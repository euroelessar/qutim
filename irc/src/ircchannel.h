/****************************************************************************
 *  ircchannel.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H

#include <qutim/conference.h>
#include "ircglobal.h"

namespace qutim_sdk_0_3 {

class ChatSession;

namespace irc {

class IrcChannelPrivate;
class IrcChannelParticipant;
class IrcAccount;

class IrcChannel : public Conference
{
	Q_OBJECT
public:
	IrcChannel(IrcAccount *account, const QString &name);
	~IrcChannel();
	virtual Buddy *me() const;
	virtual void join();
	void join(const QString &pass);
	virtual void leave();
	virtual QString id() const;
	virtual bool sendMessage(const Message &message);
	virtual QString topic() const;
	virtual ChatUnitList lowerUnits();
	const IrcAccount *account() const;
	IrcAccount *account();
	IrcChannelParticipant *participant(const QString &nick);
	QList<IrcChannelParticipant*> participants();
private slots:
	void onMyNickChanged(const QString &nick);
	void onParticipantNickChanged(const QString &nick);
	void onContactQuit(const QString &message);
private:
	friend class IrcConnection;
	void handleUserList(const QStringList &users);
	void handleJoin(const QString &nick, const QString &host);
	void handlePart(const QString &nick, const QString &message);
	void handleKick(const QString &nick, const QString &by, const QString &message);
	void handleTopic(const QString &topic);
	void handleTopicInfo(const QString &user, const QString &timeStr);
	void handleMode(const QString &who, const QString &mode, const QString &param);
	void setMode(const QString &who, QChar mode, const QString &param);
	void removeMode(const QString &who, QChar mode, const QString &param);
	void addSystemMessage(const QString &message, ChatSession *session);
	void clear(ChatSession *session);
private:
	QScopedPointer<IrcChannelPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNEL_H
