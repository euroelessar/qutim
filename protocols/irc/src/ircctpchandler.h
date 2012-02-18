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

