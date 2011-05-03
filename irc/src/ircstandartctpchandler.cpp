/****************************************************************************
 *  ircstandartctpchandler.cpp
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

#include "ircstandartctpchandler.h"
#include "ircaccount.h"
#include "ircconnection.h"
#include <qutim/libqutim_version.h>
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace irc {

IrcStandartCtpcHandler::IrcStandartCtpcHandler(IrcConnection *conn) :
	m_conn(conn)
{
	m_ctpcCmds << "PING" << "ACTION" << "CLIENTINFO" << "VERSION" << "TIME";
}

void IrcStandartCtpcHandler::handleCtpcRequest(IrcAccount *account, const QString &sender, const QString &senderHost,
											   const QString &receiver, const QString &cmd, const QString &params)
{
	Q_UNUSED(account);
	Q_UNUSED(senderHost);
	Q_UNUSED(receiver);
	if (cmd == "PING") {
		account->sendCtpcReply(sender, "PING", params);
	} else if (cmd == "ACTION") {
		m_conn->handleTextMessage(sender, senderHost, receiver, QLatin1String("/me ") + params);
	} else if (cmd == "CLIENTINFO") {
		QString params = QString("IRC plugin for qutIM %1 - http://qutim.org - Supported tags: %2")
						 .arg(versionString())
						 .arg(m_conn->supportedCtpcTags().join(","));
		account->sendCtpcReply(sender, "CLIENTINFO", params);
	} else if (cmd == "VERSION") {
		QString params = QString("IRC plugin %1, qutim %2")
						 .arg(qutimIrcVersionStr())
						 .arg(versionString());
		account->sendCtpcReply(sender, "VERSION", params);
	} else if (cmd == "TIME") {
		account->sendCtpcReply(sender, "TIME", QDateTime::currentDateTime().toString("ddd MMM dd hh:mm:ss yyyy"));
	}
}

void IrcStandartCtpcHandler::handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
												const QString &receiver, const QString &cmd, const QString &params)
{
	Q_UNUSED(account);
	Q_UNUSED(senderHost);
	Q_UNUSED(receiver);
	if (cmd == "PING") {
		QDateTime current = QDateTime::currentDateTime();
		double receivedStamp = params.toDouble();
		if (receivedStamp >= 0) {
			double currentStamp = (double)current.time().msec() / 1000 + current.toTime_t();
			double diff = currentStamp - receivedStamp;
			account->log(tr("Received CTCP-PING reply from %1: %2 seconds", "", diff)
						 .arg(sender)
						 .arg(diff, 0, 'f', 3),
						 true, "CTPC");
		}
	} else if (cmd == "CLIENTINFO" || cmd == "VERSION" || "TIME") {
		account->log(tr("Received CTCP-%1 reply from %2: %3")
					 .arg(cmd)
					 .arg(sender)
					 .arg(params),
					 true, "CTPC");
	}
}

} } // namespace namespace qutim_sdk_0_3::irc
