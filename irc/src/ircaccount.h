/****************************************************************************
 *  ircaccount.h
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

#ifndef IRCACCOUNT_H
#define IRCACCOUNT_H

#include <qutim/account.h>
#include <QHostAddress>
#include "irccontact.h"

namespace qutim_sdk_0_3 {

class ChatSession;

namespace irc {

class IrcAccountPrivate;
class IrcChannel;
class IrcProtocol;

class IrcAccount: public Account
{
	Q_OBJECT
public:
	IrcAccount(const QString &network);
	virtual ~IrcAccount();
	virtual void setStatus(Status status);
	virtual QString name() const;
	virtual ChatUnit *getUnitForSession(ChatUnit *unit);
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false);
	IrcChannel *getChannel(const QString &name, bool create = false);
	IrcContact *getContact(const QString &nick, bool create = false);
	void send(const QString &command) const;
	void setName(const QString &name) const;
	IrcProtocol *protocol();
	const IrcProtocol *protocol() const;
	ChatSession *activeSession() const;
public slots:
	void updateSettings();
protected:
	virtual bool event(QEvent *ev);
private slots:
	void onContactRemoved();
	void onContactNickChanged(const QString &nick);
private:
	friend class IrcChannelParticipant;
	friend class IrcConnection;
	QScopedPointer<IrcAccountPrivate> d;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCACCOUNT_H
