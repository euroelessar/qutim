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

#include "ircservermessagehandler.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcCommand::IrcCommand(const QString &value) :
	m_value(value)
{
	m_code = m_value.toInt();
}

IrcCommand::IrcCommand(const char *value) :
	m_value(value)
{
	m_code = m_value.toInt();
}

IrcCommand::IrcCommand(int code) :
	m_code(code)
{
	m_value = QString("%1").arg(code, 3, 10, QChar('0'));
}

IrcServerMessageHandler::~IrcServerMessageHandler()
{
}

} } // namespace qutim_sdk_0_3:: irc

