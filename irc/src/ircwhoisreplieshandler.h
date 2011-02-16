/****************************************************************************
 *  ircwhoisreplieshandler.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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
	IrcWhoisRepliesHandler();
	virtual ~IrcWhoisRepliesHandler();
	virtual void handleMessage(class IrcAccount *account, const QString &name, const QString &host,
							   const IrcCommand &cmd, const QStringList &params);
};

} } // namespace qutim_sdk_0_3::irc

#endif // WHOISREPLIESHANDLER_H
