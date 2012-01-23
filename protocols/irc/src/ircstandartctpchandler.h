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

