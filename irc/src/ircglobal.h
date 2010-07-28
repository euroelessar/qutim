/****************************************************************************
 *  ircglobal.h
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

#ifndef IRCGLOBAL_H
#define IRCGLOBAL_H

#include <qutim/libqutim_global.h>
#include <qutim/debug.h>

#define QUTIM_IRC_VERSION_STR "0.1.0.0"

namespace qutim_sdk_0_3 {

namespace irc {

QString qutimIrcVersionStr();

} } // namespace qutim_sdk_0_3::irc

#endif // IRCGLOBAL_H
