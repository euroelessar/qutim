/****************************************************************************
 *  irccptchandler.cpp
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

#include "ircctpchandler.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcCtpcHandler::~IrcCtpcHandler()
{
}

void IrcCtpcHandler::handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
										const QString &receiver, const QString &cmd, const QString &params)
{
	Q_UNUSED(account);
	Q_UNUSED(sender);
	Q_UNUSED(senderHost);
	Q_UNUSED(receiver);
	Q_UNUSED(cmd);
	Q_UNUSED(params);
}

} } // namespace qutim_sdk_0_3::irc
