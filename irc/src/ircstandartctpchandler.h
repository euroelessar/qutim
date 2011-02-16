/****************************************************************************
 *  ircstandartctpchandler.h
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

#ifndef IRCSTANDARTCTPCHANDLER_H
#define IRCSTANDARTCTPCHANDLER_H

#include "ircglobal.h"
#include "ircctpchandler.h"

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;
class IrcConnection;

class IrcStandartCtpcHandler : public QObject, public IrcCtpcHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::irc::IrcCtpcHandler)
public:
	IrcStandartCtpcHandler(IrcConnection *conn);
	void handleCtpcRequest(IrcAccount *account, const QString &sender, const QString &senderHost,
						   const QString &receiver, const QString &cmd, const QString &params);
	void handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
							const QString &receiver, const QString &cmd, const QString &params);
private:
	IrcConnection *m_conn;
};

} } // namespace namespace qutim_sdk_0_3::irc

#endif // IRCSTANDARTCTPCHANDLER_H
