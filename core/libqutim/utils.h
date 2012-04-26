/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef UTILS_H
#define UTILS_H
#include "libqutim_global.h"

namespace qutim_sdk_0_3
{

template<typename T>
Q_INLINE_TEMPLATE T sender_cast(QObject *sender)
{
	Q_ASSERT(qobject_cast<T>(sender));
	return static_cast<T>(sender);
}

class LIBQUTIM_EXPORT UrlParser
{
public:
	enum Flag {
		None = 0x00,
		Html = 0x01
	};
	Q_DECLARE_FLAGS(Flags, Flag)
	struct UrlToken
	{
		QStringRef text;
		QString url;
	};
	typedef QList<UrlToken> UrlTokenList;
	
	static UrlTokenList tokenize(const QString &text, Flags flags = None);
	static QString parseUrls(const QString &text, Flags flags = None);
private:
	UrlParser();
	~UrlParser();
};

} //namespace qutim_sdk_0_3

Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::UrlParser::Flags)

#endif // UTILS_H

