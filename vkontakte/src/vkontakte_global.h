/****************************************************************************
 *  vkontakte_global.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef VKONTAKTE_GLOBAL_H
#define VKONTAKTE_GLOBAL_H

#if defined(VKONTAKTE_LIBRARY)
#  define LIBVKONTAKTE_EXPORT Q_DECL_EXPORT
#else
#  define LIBVKONTAKTE_EXPORT Q_DECL_IMPORT
#endif

#include <qutim/libqutim_global.h>
#include <qutim/status.h>

using namespace qutim_sdk_0_3;

class QDir;

enum VConnectionState {
	Invalid = -1,
	Connected,
	Connecting,
	Disconnected,
	Authorization
};

VConnectionState LIBVKONTAKTE_EXPORT statusToState(Status::Type);
Status::Type LIBVKONTAKTE_EXPORT stateToStatus(VConnectionState state);
QDir LIBVKONTAKTE_EXPORT getAvatarsDir();

#endif // VKONTAKTE_GLOBAL_H
