/****************************************************************************
 *  ircaccount_p.h
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


#ifndef IRCACCOUNT_P_H
#define IRCACCOUNT_P_H

#include "ircaccount.h"
#include <QEvent>
#include "irccontact.h"
#include "ircgroupchatmanager.h"
#include "ui/ircconsole.h"
#include "ui/ircchannellist.h"

namespace qutim_sdk_0_3 {

namespace irc {

class IrcConnection;
class IrcContact;

struct LastCommand
{
	uint time;
	QString cmd;
};

class IrcAccountPrivate
{
public:
	IrcContact *newContact(const QString &nick, const QString &host);
	IrcChannel *newChannel(const QString &name);
	void removeContact(IrcContact *contact);
	void removeOldCommands();
	friend class IrcAccount;
	IrcAccount *q;
	IrcConnection *conn;
	QHash<QString, IrcContact *> contacts;
	QHash<QString, IrcChannel *> channels;
	QPointer<IrcConsoleFrom> consoleForm;
	QString log;
	QPointer<IrcChannelListForm> channelListForm;
	QString avatar;
	QScopedPointer<IrcGroupChatManager> groupManager;
	QList<LastCommand> lastCommands;

	static QHash<QString, QString> logMsgColors;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCACCOUNT_P_H
