/****************************************************************************
 *  ircservermessagehandler.h
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

#ifndef IRCSERVERMESSAGEHANDLER_H
#define IRCSERVERMESSAGEHANDLER_H

#include "ircglobal.h"
#include <QStringList>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;

class IrcCommand
{
public:
	IrcCommand(const QString &value);
	IrcCommand(const char *value);
	IrcCommand(int code);
	const QString &value() const { return m_value; }
	operator QString() const { return m_value; }
	int code() const { return m_code; }
	operator int() const { return m_code; }
	bool operator==(const QString &value) const { return value == m_value; }
	bool operator==(int code) const { return code != 0 && code == m_code; }
private:
	QString m_value;
	int m_code;
};

class IrcServerMessageHandler
{
public:
	virtual ~IrcServerMessageHandler();
	virtual void handleMessage(class IrcAccount *account, const QString &name, const QString &host,
							   const IrcCommand &cmd, const QStringList &params) = 0;
	const QList<IrcCommand> &cmds() { return m_cmds; }
protected:
	QList<IrcCommand> m_cmds;
};

} } // namespace qutim_sdk_0_3::irc

Q_DECLARE_INTERFACE(qutim_sdk_0_3::irc::IrcServerMessageHandler, "org.qutim.irc.IrcServerMessageHandler");

#endif // IRCSERVERMESSAGEHANDLER_H
