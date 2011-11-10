/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef TIMEMODIFIER_H
#define TIMEMODIFIER_H

#include "../messagemodifier.h"

namespace qutim_sdk_0_3
{
	LIBQUTIM_EXPORT QString convertTimeDate(const QString &mac_format, const QDateTime &datetime);
}

namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;

		class TimeModifier : public QObject, public MessageModifier
		{
			Q_OBJECT
			Q_INTERFACES(Core::AdiumChat::MessageModifier)
		public:
			TimeModifier();
			virtual ~TimeModifier();
			virtual QString getValue(const ChatSession *session, const Message &message, const QString &name, const QString &value);
			virtual QStringList supportedNames() const;
		private:
			QString m_datetimeFormat;
		};
	}
}
#endif // TIMEMODIFIER_H

