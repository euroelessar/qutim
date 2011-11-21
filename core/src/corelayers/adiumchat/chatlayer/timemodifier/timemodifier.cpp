/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "timemodifier.h"
#include <qutim/configbase.h>
#include <QDateTime>

namespace Core
{
namespace AdiumChat
{
TimeModifier::TimeModifier()
{
	ConfigGroup adium_chat = Config("appearance").group("adiumChat/style");
	m_datetimeFormat = adium_chat.value<QString>("datetimeFormat","hh:mm:ss dd/MM/yyyy");
}

TimeModifier::~TimeModifier()
{
}

QString TimeModifier::getValue(const ChatSession *, const Message &message, const QString &name, const QString &value)
{
	if(name == QLatin1String("time") || name == QLatin1String("timeOpened"))
	{
		QDateTime time = message.time();
		return convertTimeDate(value.isEmpty() ? m_datetimeFormat : value, time.isValid() ? time : QDateTime::currentDateTime());
	}
	return value;
}

QStringList TimeModifier::supportedNames() const
{
	return QStringList() << QLatin1String("time") << QLatin1String("timeOpened");
}
}
}

