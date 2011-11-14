/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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
} //namespace qutim_sdk_0_3

#endif // UTILS_H

