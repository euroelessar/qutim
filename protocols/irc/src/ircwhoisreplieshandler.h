/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCWHOISREPLIESHANDLER_H
#define IRCWHOISREPLIESHANDLER_H

#include "ircservermessagehandler.h"

namespace qutim_sdk_0_3 {

namespace irc {

class IrcWhoisRepliesHandler : public QObject, public IrcServerMessageHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::irc::IrcServerMessageHandler)
public:
	IrcWhoisRepliesHandler(QObject *parent = 0);
	virtual ~IrcWhoisRepliesHandler();
	virtual void handleMessage(class IrcAccount *account, const QString &name, const QString &host,
							   const IrcCommand &cmd, const QStringList &params);
};

} } // namespace qutim_sdk_0_3::irc

#endif // WHOISREPLIESHANDLER_H

