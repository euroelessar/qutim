/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

