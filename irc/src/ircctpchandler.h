/****************************************************************************
 *  irccptchandler.h
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

#ifndef IRCCTPCHANDLER_H
#define IRCCTPCHANDLER_H

#include "ircglobal.h"
#include <QStringList>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;

class IrcCtpcHandler
{
public:
	virtual ~IrcCtpcHandler();
	virtual void handleCtpcRequest(IrcAccount *account, const QString &sender, const QString &senderHost,
								   const QString &receiver, const QString &cmd, const QString &params) = 0;
	virtual void handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
									const QString &receiver, const QString &cmd, const QString &params) = 0;
	QStringList ctpcCmds() { return m_ctpcCmds; }
protected:
	QStringList m_ctpcCmds;
};

} } // namespace qutim_sdk_0_3::irc

Q_DECLARE_INTERFACE(qutim_sdk_0_3::irc::IrcCtpcHandler, "org.qutim.irc.IrcCtpcHandler");

#endif // IRCCTPCHANDLER_H
