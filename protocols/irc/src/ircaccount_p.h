/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/


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

