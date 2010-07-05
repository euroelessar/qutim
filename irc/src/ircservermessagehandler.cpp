/****************************************************************************
 *  ircservermessagehandler.cpp
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
