/****************************************************************************
 *  utils.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef UTILS_H
#define UTILS_H
#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
template<typename T>
Q_INLINE_TEMPLATE T sender_cast(QObject *sender)
{
#ifdef QT_DEBUG
	T t = qobject_cast<T>(sender);
	Q_ASSERT(t);
	return t;
#else
	return static_cast<T>(static_cast<void*>(sender));
#endif
}
} //namespace qutim_sdk_0_3

#endif // UTILS_H
